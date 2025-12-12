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
        printf(COLOR_YELLOW "   Latency:     " COLOR_RESET "%.2f ms\n", result->latency_ms);
    }
    
    if (result->download_speed_mbps > 0) {
        printf(COLOR_BLUE "   Download:    " COLOR_RESET COLOR_BOLD "%.2f Mbps" COLOR_RESET "\n", 
               result->download_speed_mbps);
    }
    
    if (result->upload_speed_mbps > 0) {
        printf(COLOR_MAGENTA "   Upload:      " COLOR_RESET COLOR_BOLD "%.2f Mbps" COLOR_RESET "\n", 
               result->upload_speed_mbps);
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