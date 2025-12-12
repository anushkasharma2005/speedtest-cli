#include "../include/display.h"
#include <stdio.h>
#include <string.h>

void display_header(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD);
    printf("╔════════════════════════════════════════╗\n");
    printf("║        SPEEDTEST CLI v1.0              ║\n");
    printf("║     Network Speed Test Tool            ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    printf("\n");
}

void display_ip_info(const IPInfo *info) {
    if (!info->success) {
        display_error("Failed to fetch IP information");
        return;
    }
    
    printf(COLOR_BOLD "\n Connection Information:\n" COLOR_RESET);
    printf("─────────────────────────────────────────\n");
    printf(COLOR_GREEN "  IP Address:  " COLOR_RESET "%s\n", info->ip);
    printf(COLOR_GREEN "  ISP:         " COLOR_RESET "%s\n", info->isp);
    printf(COLOR_GREEN "  Location:    " COLOR_RESET "%s, %s, %s\n", 
           info->city, info->region, info->country);
    printf(COLOR_GREEN "  Timezone:    " COLOR_RESET "%s\n", info->timezone);
}

void display_speed_results(const SpeedTestResult *result) {
    if (!result->success) {
        display_error("Speed test failed");
        return;
    }
    
    printf(COLOR_BOLD " Final Results:\n" COLOR_RESET);
    printf("═════════════════════════════════════════\n");
    
    if (result->latency_ms > 0) {
        printf(COLOR_YELLOW "   Latency:     " COLOR_RESET "%.2f ms", result->latency_ms);
        // Latency interpretation
        if (result->latency_ms < 20) {
            printf(COLOR_GREEN " (Excellent)\n");
            printf("                Ideal for competitive gaming, real-time trading\n");
        } else if (result->latency_ms < 50) {
            printf(COLOR_GREEN " (Good)\n");
            printf("                Great for online gaming, video calls, browsing\n");
        } else if (result->latency_ms < 100) {
            printf(COLOR_YELLOW " (Fair)\n");
            printf("                Acceptable for casual gaming, may notice delays\n");
        } else if (result->latency_ms < 200) {
            printf(COLOR_RED " (Poor)\n");
            printf("                Noticeable lag in games, slower response times\n");
        } else {
            printf(COLOR_RED " (Very Poor)\n");
            printf("                Severe lag, not suitable for real-time activities\n");
        }
    }
    
    if (result->download_speed_mbps > 0) {
        printf(COLOR_BLUE "   Download:    " COLOR_RESET COLOR_BOLD "%.2f Mbps" COLOR_RESET, 
               result->download_speed_mbps);
        // Download interpretation
        if (result->download_speed_mbps >= 100) {
            printf(COLOR_GREEN " (Excellent)\n");
            printf("                Multiple 4K streams, large file downloads, smooth gaming\n");
        } else if (result->download_speed_mbps >= 50) {
            printf(COLOR_GREEN " (Very Good)\n");
            printf("                2-3 simultaneous 4K streams, fast downloads\n");
        } else if (result->download_speed_mbps >= 25) {
            printf(COLOR_YELLOW " (Good)\n");
            printf("                Single 4K stream, HD video calls, online gaming\n");
        } else if (result->download_speed_mbps >= 10) {
            printf(COLOR_YELLOW " (Fair)\n");
            printf("                HD streaming (1080p), web browsing, light downloads\n");
        } else if (result->download_speed_mbps >= 5) {
            printf(COLOR_RED " (Moderate)\n");
            printf("                SD streaming only, slow downloads, basic tasks\n");
        } else {
            printf(COLOR_RED " (Poor)\n");
            printf("                Very slow, only basic web browsing possible\n");
        }
    }
    
    if (result->upload_speed_mbps > 0) {
        printf(COLOR_MAGENTA "   Upload:      " COLOR_RESET COLOR_BOLD "%.2f Mbps" COLOR_RESET, 
               result->upload_speed_mbps);
        // Upload interpretation
        if (result->upload_speed_mbps >= 50) {
            printf(COLOR_GREEN " (Excellent)\n");
            printf("                4K live streaming, large file uploads, cloud backups\n");
        } else if (result->upload_speed_mbps >= 25) {
            printf(COLOR_GREEN " (Very Good)\n");
            printf("                HD streaming, video conferencing, file sharing\n");
        } else if (result->upload_speed_mbps >= 10) {
            printf(COLOR_YELLOW " (Good)\n");
            printf("                HD video calls, moderate file uploads\n");
        } else if (result->upload_speed_mbps >= 5) {
            printf(COLOR_YELLOW " (Fair)\n");
            printf("                SD video calls, small file uploads\n");
        } else if (result->upload_speed_mbps >= 3) {
            printf(COLOR_RED " (Moderate)\n");
            printf("                Slow uploads, basic email attachments only\n");
        } else {
            printf(COLOR_RED " (Poor)\n");
            printf("                Very slow uploads, limited functionality\n");
        }
    }
    
    printf("═════════════════════════════════════════\n\n");
}

void display_progress(const char *test_name, int percent) {
    clear_line();
    printf("\r" COLOR_CYAN "%s: " COLOR_RESET "[", test_name);
    
    int bars = percent / 2;
    for (int i = 0; i < 50; i++) {
        if (i < bars) {
            printf("█");
        } else {
            printf("░");
        }
    }
    
    printf("] %d%%", percent);
    fflush(stdout);
}

void display_error(const char *message) {
    printf(COLOR_RED " Error: " COLOR_RESET "%s\n", message);
}

void clear_line(void) {
    printf("\r\033[K");
}
