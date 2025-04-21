#include <https_post.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

void print_response(char* response){
    if (response == NULL) {
        fprintf(stderr, "No response received.\n");
        return;
    }

    FILE* fp = fopen("log.txt", "w");
    if (fp == NULL) {
        perror("Failed to open log.txt for writing.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "Response: %s\n", response);
    fclose(fp);
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

    //print_response(response);

    return response;
}

struct xml_inforamtion* extract_xml(char* start_ptr, int* xml_info_size) {
    struct xml_inforamtion* xml_info = malloc(sizeof(struct xml_inforamtion*) * 1024);

    char* end_ptr = start_ptr + strlen(start_ptr);
    char* port_tags_start = NULL;
    char* port_tags_end = NULL;
    char* port_tag_name = NULL;
    char closing_tag[MAX_TAG_SIZE];
    
    char* value = NULL;
    while(1) {
        //Find the start of a tags
        port_tags_start = strstr(start_ptr, "<");
        if(!port_tags_start || port_tags_start > end_ptr) {
            break;
        }

        //Find the start of a tag
        port_tags_end = strstr(++port_tags_start, ">");
        if(!port_tags_end || port_tags_end > end_ptr) {
            break;
        }

        port_tag_name = malloc(port_tags_end - port_tags_start + 1);
        if(port_tag_name == NULL) {
            perror("Not enough memory (malloc returned NULL)\n");
            exit(EXIT_FAILURE);
        }
        strncpy(port_tag_name, port_tags_start, port_tags_end - port_tags_start);
        port_tag_name[port_tags_end - port_tags_start] = '\0';


        snprintf(closing_tag, sizeof(closing_tag), "</%s>", port_tag_name);
        const char* closing_tag_start = strstr(port_tags_end+1, closing_tag);
        if (strchr(port_tag_name , '/') != NULL) {
            start_ptr = port_tags_end + strlen(closing_tag) + 1;
            free(port_tag_name);
            port_tag_name = NULL;
            continue;
        }
        

        value = NULL;
        // Extract the value between the tags
        if (closing_tag_start) {
            size_t value_length = closing_tag_start - (port_tags_end + 1);
            value = malloc(value_length + 1);
            if (value == NULL) {
                perror("Not enough memory for tag value");
                exit(EXIT_FAILURE);
            }
            strncpy(value, port_tags_end + 1, value_length);
            if (value[0] == '<') {
                value[0] = '\0';
            } else {
                value[value_length] = '\0';
            }
        }

        
        if (value != NULL && strlen(value) > 0) {
            start_ptr = port_tags_end + 1 + strlen(value) + strlen(closing_tag);
        } else {
            start_ptr = port_tags_end + 1;
            free(port_tag_name);
            free(value);
            port_tag_name = NULL;
            value = NULL;
            continue;
        }
        
        // fprintf(stderr, "port_tag_name:%s\n", port_tag_name);
        // if (value != NULL) {
        //     fprintf(stderr, "value:%s\n", value);
        // }

        xml_info[(*xml_info_size)].post = port_tag_name;
        xml_info[(*xml_info_size)].value = value;
        (*xml_info_size)++;
    }

    return xml_info;
}

struct xml_inforamtion* extract_xml_values(char *xml_string, int *xml_data_size) {
    struct xml_inforamtion* xml_data = NULL;
    char* start_ptr = strdup(xml_string);    
    xml_data = extract_xml(start_ptr, xml_data_size);
    
    free((char*)xml_string);
    free(start_ptr);
    return xml_data;
}