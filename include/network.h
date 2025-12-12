#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

// Structure to hold speed test results
typedef struct {
    double download_speed_mbps;
    double upload_speed_mbps;
    double latency_ms;
    int success;
} SpeedTestResult;

// Callback structure for tracking progress
typedef struct {
    size_t total_bytes;
    double start_time;
} TransferInfo;

// Initialize network module
int network_init(void);

// Cleanup network module
void network_cleanup(void);

// Run download speed test
double test_download_speed(const char *url, size_t expected_size);

// Run upload speed test
double test_upload_speed(const char *url, size_t data_size);

// Measure latency/ping
double test_latency(const char *url);

// Run full speed test
SpeedTestResult run_speed_test(void);

// Get current time in seconds
double get_current_time(void);

#endif // NETWORK_H