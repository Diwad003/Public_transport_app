#ifndef HTTPS_POST_H
#define HTTPS_POST_H

#include <curl/curl.h>
#include <libxml/parser.h>

#define MAX_AMOUNT_OF_CHILDREN 100
#define MAX_TAG_SIZE 100

// Structure to hold response data
struct Memory {
    char *response;
    size_t size;
};

struct xml_inforamtion {
    char* post;
    char* value;
};

char* send_and_receive_request(const char *url, char *request_body);
struct xml_inforamtion* extract_xml_values(char *xml_string, int *xml_data_size);

#endif // HTTPS_POST_H