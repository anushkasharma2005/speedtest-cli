#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/network.h"
#include "../include/ip_info.h"
#include "../include/display.h"



void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("\nOptions:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --version  Show version information\n");
    printf("  -q, --quick    Quick test (download only)\n");
    printf("\n");
}

void print_version(void) {
    printf("Speedtest CLI v1.0\n");
    printf("A lightweight network speed test tool\n");
}


// Global flag for quick mode
int g_quick_mode = 0;


int main(int argc, char *argv[]) {
    // int quick_mode = 0;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quick") == 0) {
            g_quick_mode = 1;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Initialize network module
    if (!network_init()) {
        display_error("Failed to initialize network module");
        return 1;
    }
    
    // Display header
    display_header();
    
    // Fetch IP and ISP information
    printf(COLOR_CYAN " Fetching connection information..." COLOR_RESET "\n");
    IPInfo ip_info = get_ip_info();
    display_ip_info(&ip_info);
    
    // Run speed test
    SpeedTestResult result = run_speed_test();
    
    // Display final results
    printf("\n");
    display_speed_results(&result);
    
    // Cleanup
    network_cleanup();
    free_ip_info(&ip_info);
    
    return result.success ? 0 : 1;
}