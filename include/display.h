#ifndef DISPLAY_H
#define DISPLAY_H

#include "network.h"
#include "ip_info.h"

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

// Display functions
void display_header(void);
void display_ip_info(const IPInfo *info);
void display_speed_results(const SpeedTestResult *result);
void display_progress(const char *test_name, int percent);
void display_error(const char *message);
void clear_line(void);

#endif // DISPLAY_H