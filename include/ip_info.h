#ifndef IP_INFO_H
#define IP_INFO_H

// Structure to hold IP and ISP information
typedef struct {
    char ip[46];           // IPv4 or IPv6 address
    char isp[256];         // Internet Service Provider
    char city[128];        // City
    char region[128];      // State/Region
    char country[128];     // Country
    char timezone[64];     // Timezone
    int success;           // Whether fetch was successful
} IPInfo;

// Fetch IP and ISP information
IPInfo get_ip_info(void);

// Free any allocated resources (if needed in future)
void free_ip_info(IPInfo *info);

#endif // IP_INFO_H