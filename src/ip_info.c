#include "../include/ip_info.h"
#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define IP_API_URL "http://ip-api.com/json/"

// Buffer to store API response
typedef struct {
    char *data;
    size_t size;
} MemoryBuffer;

// Callback to store response in memory
static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryBuffer *mem = (MemoryBuffer *)userp;
    
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Not enough memory\n");
        return 0;
    }
    
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    
    return realsize;
}

IPInfo get_ip_info(void) {
    IPInfo info = {0};
    CURL *curl;
    CURLcode res;
    MemoryBuffer chunk = {0};
    
    curl = curl_easy_init();
    if (!curl) {
        info.success = 0;
        return info;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, IP_API_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    
    res = curl_easy_perform(curl);
    
    if (res == CURLE_OK && chunk.data) {
        // Parse JSON response
        struct json_object *parsed_json;
        struct json_object *query, *isp, *city, *region, *country, *timezone;
        
        parsed_json = json_tokener_parse(chunk.data);
        
        if (parsed_json) {
            // Extract fields
            if (json_object_object_get_ex(parsed_json, "query", &query)) {
                strncpy(info.ip, json_object_get_string(query), sizeof(info.ip) - 1);
            }
            
            if (json_object_object_get_ex(parsed_json, "isp", &isp)) {
                strncpy(info.isp, json_object_get_string(isp), sizeof(info.isp) - 1);
            }
            
            if (json_object_object_get_ex(parsed_json, "city", &city)) {
                strncpy(info.city, json_object_get_string(city), sizeof(info.city) - 1);
            }
            
            if (json_object_object_get_ex(parsed_json, "regionName", &region)) {
                strncpy(info.region, json_object_get_string(region), sizeof(info.region) - 1);
            }
            
            if (json_object_object_get_ex(parsed_json, "country", &country)) {
                strncpy(info.country, json_object_get_string(country), sizeof(info.country) - 1);
            }
            
            if (json_object_object_get_ex(parsed_json, "timezone", &timezone)) {
                strncpy(info.timezone, json_object_get_string(timezone), sizeof(info.timezone) - 1);
            }
            
            info.success = 1;
            json_object_put(parsed_json);
        }
    }
    
    if (chunk.data) {
        free(chunk.data);
    }
    
    curl_easy_cleanup(curl);
    return info;
}

void free_ip_info(IPInfo *info) {
    // Currently nothing to free, but kept for future expansion
    (void)info;
}