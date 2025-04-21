#ifndef HTTPS_POST_H
#define HTTPS_POST_H

#include <curl/curl.h>
#include <libxml/parser.h>

#define MAX_AMOUNT_OF_CHILDREN 100

// Structure to hold response data
struct Memory {
    char *response;
    size_t size;
};

char* send_and_receive_request(const char *url, char *request_body);

#endif // HTTPS_POST_H