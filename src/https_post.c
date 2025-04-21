#include <https_post.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + real_size + 1);
    if (!ptr) {
        perror("Not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, real_size);
    mem->size += real_size;
    mem->response[mem->size] = '\0';

    return real_size;
}

char* send_and_receive_request(const char *url, char *request_body) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    struct Memory chunk;
    char* response = NULL;

    chunk.response = malloc(1);  // will be grown as needed by write_callback
    if (chunk.response == NULL) {
        perror("Not enough memory (malloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }
    chunk.size = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);//SETUP URL
        headers = curl_slist_append(headers, "Content-Type: text/xml");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//SETUP HEADERS

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body);//SETUP DATA SEND
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);//CALLBACK
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);//RECIEVE DATA
        
        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            response = malloc(chunk.size + 1);
            if (response == NULL) {
                perror("Not enough memory (malloc returned NULL)\n");
                exit(EXIT_FAILURE);
            }
            memcpy(response, chunk.response, chunk.size);
            response[chunk.size] = '\0'; // Initialize the response string
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        perror("Failed to initialize CURL.\n");
        exit(EXIT_FAILURE);
    }

    // Free the response memory
    free(chunk.response);
    curl_global_cleanup();
    return response;
}

char* find_str(char* str, char* c, int forward_search) {
    char* result_str = NULL;
    
    if (forward_search) {
        result_str = strstr(str, c);
        if (result_str == NULL) {
            return NULL;
        }
    } else {
        result_str = str;
        while (*(--result_str) != *c) {
            if (result_str == NULL) {
                return NULL;
            }
        }
        result_str++;
    }

    return result_str;
}

struct xml_info* extract_xml_values(const char *xml_string, int *xml_data_size) {
    char* start_ptr = (char*)xml_string;
    char* post_tag_start, * post_tag_end, *value_tag_start, *value_tag_end;
    char* port, * value;
    int i = 0;
    struct xml_info* xml_data = malloc(sizeof(struct xml_info));
    if (xml_data == NULL) {
        perror("Not enough memory (malloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    do {
        value_tag_end = find_str(start_ptr, "</", 1);
        value_tag_start = find_str(value_tag_end, ">", 0);
        post_tag_start = find_str(value_tag_end, "<", 0);
        post_tag_end = find_str(post_tag_start, ">", 1);
        if (value_tag_start == NULL || value_tag_end == NULL
            || post_tag_start == NULL || post_tag_end == NULL) {
            break; // No closing tag found
        }


        port = malloc(post_tag_end - post_tag_start + 1);
        if (port == NULL) {
            perror("Not enough memory (malloc returned NULL)\n");
            exit(EXIT_FAILURE);
        }
        strncpy(port, post_tag_start, post_tag_end - post_tag_start);
        port[post_tag_end - post_tag_start] = '\0';
        //If first character is '/' then we have found a closing tag, which means no more information
        if(post_tag_start[0] == '/')
        {
            free(port);
            break;
        }
        
        xml_data = realloc(xml_data, sizeof(struct xml_info) * (i + 1));
        if (xml_data == NULL) {
            perror("Not enough memory (realloc returned NULL)\n");
            exit(EXIT_FAILURE);
        }
        xml_data[i].post = port;
        
        
        value = malloc(value_tag_end - value_tag_start + 1);
        if (value == NULL) {
            perror("Not enough memory (malloc returned NULL)\n");
            exit(EXIT_FAILURE);
        }
        strncpy(value, value_tag_start, value_tag_end - value_tag_start);
        value[value_tag_end - value_tag_start] = '\0';
        xml_data[i].value = value;
        
        // fprintf(stderr, "port:%s\n", port);
        // fprintf(stderr, "value:%s\n", value);
        
        start_ptr = value_tag_end+2;
        i++;
    }  while (start_ptr != NULL);

    free((void*)xml_string);
    *xml_data_size = i;

    return xml_data;
}