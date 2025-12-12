#include "../include/network.h"
#include "../include/display.h"
#include <curl/curl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

// External quick mode flag from main.c
extern int g_quick_mode;

// Better test servers - includes Asian/Global CDNs
static const char *DOWNLOAD_TEST_URLS[] = {
    // Cloudflare (has edge servers in India)
    "https://speed.cloudflare.com/__down?bytes=100000000",
    // Fast.com Netflix (global CDN, good in India)  
    "https://ipv4-c001-bom001-jiocinema-isp.1.oca.nflxvideo.net/speedtest/test1mb.bin",
    // Singapore servers (closer to India)
    "http://speedtest.sin1.sg.leaseweb.net/10mb.bin",
    // Fallback European servers
    "http://speedtest.tele2.net/100MB.zip",
    "http://proof.ovh.net/files/100Mb.dat",
    NULL
};

// Multiple upload endpoints to test
static const char *UPLOAD_TEST_URLS[] = {
    "https://speed.cloudflare.com/__up",
    NULL
};

#define NUM_PARALLEL_CONNECTIONS 8
#define TEST_DURATION_SECONDS 12

// Shared data structure for threads
typedef struct {
    const char *url;
    atomic_size_t bytes_transferred;
    double start_time;
    volatile int running;
    volatile int success;
} ThreadData;

// Global shared counter for all threads
static atomic_size_t g_total_bytes = 0;
static volatile int g_test_running = 0;

// Get current time
double get_current_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Discard callback that updates atomic counter
static size_t download_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    (void)contents;
    size_t realsize = size * nmemb;
    atomic_size_t *bytes = (atomic_size_t *)userp;
    atomic_fetch_add(bytes, realsize);
    atomic_fetch_add(&g_total_bytes, realsize);
    return realsize;
}

// Simple discard callback for latency tests
static size_t discard_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    (void)contents;
    (void)userp;
    return size * nmemb;
}

// Progress callback that checks if test should stop
static int check_stop_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                               curl_off_t ultotal, curl_off_t ulnow) {
    (void)dltotal; (void)dlnow; (void)ultotal; (void)ulnow;
    (void)clientp;
    return g_test_running ? 0 : 1;
}

// Thread function for parallel download
static void *download_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if (!curl) {
        data->success = 0;
        data->running = 0;
        return NULL;
    }
    
    data->running = 1;
    data->success = 0;
    atomic_store(&data->bytes_transferred, 0);
    
    curl_easy_setopt(curl, CURLOPT_URL, data->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data->bytes_transferred);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, TEST_DURATION_SECONDS + 10);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 512000L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, check_stop_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, NULL);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    
    res = curl_easy_perform(curl);
    
    data->success = (res == CURLE_OK || res == CURLE_ABORTED_BY_CALLBACK);
    data->running = 0;
    
    curl_easy_cleanup(curl);
    return NULL;
}

int network_init(void) {
    return curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK ? 1 : 0;
}

void network_cleanup(void) {
    curl_global_cleanup();
}

// Find best server by latency
static const char *find_best_server(double *out_latency) {
    double best_latency = 999999.0;
    int best_index = 0;
    
    printf("   Finding best server");
    fflush(stdout);
    
    for (int i = 0; DOWNLOAD_TEST_URLS[i] != NULL; i++) {
        CURL *curl = curl_easy_init();
        if (!curl) continue;
        
        printf(".");
        fflush(stdout);
        
        curl_easy_setopt(curl, CURLOPT_URL, DOWNLOAD_TEST_URLS[i]);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_callback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
        
        double start = get_current_time();
        CURLcode res = curl_easy_perform(curl);
        double latency = (get_current_time() - start) * 1000.0;
        
        if (res == CURLE_OK && latency < best_latency) {
            best_latency = latency;
            best_index = i;
        }
        
        curl_easy_cleanup(curl);
    }
    
    *out_latency = best_latency;
    printf(" Server %d (%.0fms)\n", best_index + 1, best_latency);
    return DOWNLOAD_TEST_URLS[best_index];
}

double test_download_speed(const char *url, size_t expected_size) {
    (void)expected_size;
    
    pthread_t threads[NUM_PARALLEL_CONNECTIONS];
    ThreadData thread_data[NUM_PARALLEL_CONNECTIONS];
    
    atomic_store(&g_total_bytes, 0);
    g_test_running = 1;
    
    printf("   Testing download (%d connections)...\n", NUM_PARALLEL_CONNECTIONS);
    
    double global_start = get_current_time();
    
    for (int i = 0; i < NUM_PARALLEL_CONNECTIONS; i++) {
        thread_data[i].url = url;
        atomic_store(&thread_data[i].bytes_transferred, 0);
        thread_data[i].start_time = global_start;
        thread_data[i].running = 0;
        thread_data[i].success = 0;
        pthread_create(&threads[i], NULL, download_thread, &thread_data[i]);
    }
    
    usleep(200000);
    
    size_t last_bytes = 0;
    double last_time = global_start;
    double speed_samples[30];
    int sample_count = 0;
    
    while (1) {
        usleep(400000);
        
        double current_time = get_current_time();
        double elapsed = current_time - global_start;
        
        size_t current_bytes = atomic_load(&g_total_bytes);
        
        double interval = current_time - last_time;
        size_t interval_bytes = current_bytes - last_bytes;
        double instant_speed = 0.0;
        
        if (interval > 0 && interval_bytes > 0) {
            instant_speed = ((double)interval_bytes * 8.0 / interval) / 1000000.0;
            
            if (elapsed > 2.0 && sample_count < 30) {
                speed_samples[sample_count++] = instant_speed;
            }
        }
        
        int percent = (int)((elapsed / TEST_DURATION_SECONDS) * 100);
        if (percent > 100) percent = 100;
        
        printf("\r\033[K   Download: %6.2f Mbps [%3d%%] [", instant_speed, percent);
        int bars = percent / 2;
        for (int i = 0; i < 50; i++) {
            if (i < bars) printf("=");
            else if (i == bars) printf(">");
            else printf(" ");
        }
        printf("]");
        fflush(stdout);
        
        last_bytes = current_bytes;
        last_time = current_time;
        
        if (elapsed >= TEST_DURATION_SECONDS) {
            g_test_running = 0;
            break;
        }
    }
    
    for (int i = 0; i < NUM_PARALLEL_CONNECTIONS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double final_speed = 0.0;
    
    if (sample_count > 0) {
        for (int i = 0; i < sample_count - 1; i++) {
            for (int j = i + 1; j < sample_count; j++) {
                if (speed_samples[i] > speed_samples[j]) {
                    double tmp = speed_samples[i];
                    speed_samples[i] = speed_samples[j];
                    speed_samples[j] = tmp;
                }
            }
        }
        
        int idx_75 = (sample_count * 3) / 4;
        if (idx_75 >= sample_count) idx_75 = sample_count - 1;
        
        double sum = 0.0;
        int count = 0;
        for (int i = idx_75; i < sample_count; i++) {
            sum += speed_samples[i];
            count++;
        }
        if (count > 0) {
            final_speed = sum / count;
        } else {
            final_speed = speed_samples[sample_count / 2];
        }
    } else {
        size_t total = atomic_load(&g_total_bytes);
        double duration = get_current_time() - global_start;
        if (duration > 2.0) {
            final_speed = ((double)total * 8.0 / duration) / 1000000.0;
        }
    }
    
    printf("\r\033[K   Download: %6.2f Mbps [100%%] [==================================================] DONE\n", final_speed);
    
    return final_speed;
}

// Upload read callback
static size_t upload_read_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
    size_t *remaining = (size_t *)userp;
    size_t to_send = size * nmemb;
    if (to_send > *remaining) to_send = *remaining;
    if (to_send > 0) {
        memset(ptr, 'X', to_send);
        *remaining -= to_send;
    }
    return to_send;
}

// Upload progress data
typedef struct {
    double start_time;
    size_t total_size;
    double last_speed;
} UploadProgress;

// Upload progress callback
static int upload_progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                                    curl_off_t ultotal, curl_off_t ulnow) {
    (void)dltotal; (void)dlnow;
    
    UploadProgress *prog = (UploadProgress *)clientp;
    double elapsed = get_current_time() - prog->start_time;
    
    if (elapsed > 0.3 && ulnow > 0) {
        double speed_mbps = ((double)ulnow * 8.0 / elapsed) / 1000000.0;
        prog->last_speed = speed_mbps;
        int percent = ultotal > 0 ? (int)((ulnow * 100) / ultotal) : 0;
        
        printf("\r\033[K   Upload:   %6.2f Mbps [%3d%%] [", speed_mbps, percent);
        int bars = percent / 2;
        for (int i = 0; i < 50; i++) {
            if (i < bars) printf("=");
            else if (i == bars) printf(">");
            else printf(" ");
        }
        printf("]");
        fflush(stdout);
    }
    
    return 0;
}

double test_upload_speed(const char *url, size_t data_size) {
    CURL *curl;
    CURLcode res;
    size_t upload_remaining = data_size;
    double speed_mbps = 0.0;
    
    curl = curl_easy_init();
    if (!curl) return -1.0;
    
    UploadProgress prog = { get_current_time(), data_size, 0.0 };
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)data_size);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, upload_read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_remaining);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_callback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, upload_progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    
    printf("   Testing upload (%zu MB)...\n", data_size / (1024 * 1024));
    
    res = curl_easy_perform(curl);
    double end_time = get_current_time();
    
    if (res == CURLE_OK) {
        double duration = end_time - prog.start_time;
        speed_mbps = ((double)data_size * 8.0 / duration) / 1000000.0;
        printf("\r\033[K   Upload:   %6.2f Mbps [100%%] [==================================================] DONE\n", speed_mbps);
    } else {
        printf("\r\033[K   Upload:   Failed (%s)\n", curl_easy_strerror(res));
    }
    
    curl_easy_cleanup(curl);
    return speed_mbps;
}

double test_latency(const char *url) {
    CURL *curl;
    double latencies[10];
    int successful_pings = 0;
    
    printf("   Testing latency... ");
    fflush(stdout);
    
    for (int i = 0; i < 10; i++) {
        curl = curl_easy_init();
        if (!curl) continue;
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_callback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
        curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 0L);
        
        double start = get_current_time();
        CURLcode res = curl_easy_perform(curl);
        double latency = (get_current_time() - start) * 1000.0;
        
        if (res == CURLE_OK) {
            latencies[successful_pings++] = latency;
        }
        
        curl_easy_cleanup(curl);
        usleep(50000);
    }
    
    if (successful_pings == 0) {
        printf("Failed\n");
        return -1.0;
    }
    
    for (int i = 0; i < successful_pings - 1; i++) {
        for (int j = i + 1; j < successful_pings; j++) {
            if (latencies[i] > latencies[j]) {
                double tmp = latencies[i];
                latencies[i] = latencies[j];
                latencies[j] = tmp;
            }
        }
    }
    
    double min_latency = latencies[0];
    printf("%.2f ms\n", min_latency);
    
    return min_latency;
}

SpeedTestResult run_speed_test(void) {
    SpeedTestResult result = {0.0, 0.0, 0.0, 0};
    
    printf(COLOR_BOLD "\n Running Speed Tests:\n" COLOR_RESET);
    printf("─────────────────────────────────────────────────────────────────────────────────────────────\n");
    
    // Find best server
    double server_latency;
    const char *best_server = find_best_server(&server_latency);
    
    // Test latency
    result.latency_ms = test_latency("https://www.google.co.in");
    
    // Test download
    printf("\n");
    result.download_speed_mbps = test_download_speed(best_server, 0);
    
    // Test upload (skip if quick mode)
    if (!g_quick_mode) {
        printf("\n");
        result.upload_speed_mbps = test_upload_speed(UPLOAD_TEST_URLS[0], 25 * 1024 * 1024);
    } else {
        printf("\n   Upload: Skipped (quick mode)\n");
        result.upload_speed_mbps = 0.0;
    }
    
    printf("─────────────────────────────────────────────────────────────────────────────────────────────\n");
    
    result.success = (result.download_speed_mbps > 0);
    
    return result;
}