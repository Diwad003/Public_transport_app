#ifndef HTTPS_POST_H
#define HTTPS_POST_H

#include <curl/curl.h>
#include <libxml/parser.h>

// Structure to hold response data
struct Memory {
    char *response;
    size_t size;
};

struct xml_info {
    char* post;
    char* value;
};

char* send_and_receive_request(const char *url, char *request_body);
struct xml_info* extract_xml_values(const char *xml_string, int *xml_data_size);

#endif // HTTPS_POST_H