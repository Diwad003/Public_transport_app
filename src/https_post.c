#include <https_post.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

    //print_response(response);

    return response;
}