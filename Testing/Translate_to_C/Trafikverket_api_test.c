#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// Structure to hold response data
struct Memory {
    char *response;
    size_t size;
};

// Callback for writing received data into our Memory struct
static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + real_size + 1);
    if (!ptr) {
        // out of memory!
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, real_size);
    mem->size += real_size;
    mem->response[mem->size] = '\0';

    return real_size;
}

// Pretty-print XML string using libxml2
void print_formatted_xml(const char *xml_str) {
    xmlDocPtr doc = xmlReadMemory(xml_str, strlen(xml_str), "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse XML response.\n");
        return;
    }

    xmlChar *formatted = NULL;
    int size = 0;
    xmlDocDumpFormatMemoryEnc(doc, &formatted, &size, "UTF-8", 1);
    if (formatted) {
        printf("%s", (char *)formatted);
        xmlFree(formatted);
    }

    xmlFreeDoc(doc);
}

int send_and_receive_request(const char *url, const char *request_body) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    struct Memory chunk;

    chunk.response = malloc(1);  // will be grown as needed by write_callback
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set headers
        headers = curl_slist_append(headers, "Content-Type: text/xml");
        headers = curl_slist_append(headers, "Referer: http://www.example.com");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body);

        // Set write callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        printf("Fetching data...\n");
        
        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("%s", (char *)chunk.response);
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize CURL.\n");
    }

    free(chunk.response);
    curl_global_cleanup();
    return 0;
}

int main(void) {
    const char *request_body =
        "<REQUEST>"
        "<LOGIN authenticationkey='ec3d22b6a975450c9e817bc0c7b905ea'/>"
        "<QUERY objecttype='TrainStation' namespace='rail.infrastructure' schemaversion='1.5' limit='10'>"
        "<FILTER>" 
        "<EQ name='AdvertisedLocationName' value='Lund C'/>"
        "</FILTER>"
        "<INCLUDE>LocationSignature</INCLUDE>"
        "<INCLUDE>PlatformLine</INCLUDE>"
        "<INCLUDE>AdvertisedLocationName</INCLUDE>"
        "<INCLUDE>CountryCode</INCLUDE>"
        "<INCLUDE>PrimaryLocationCode</INCLUDE>"
        "</QUERY>"
        "</REQUEST>";

    send_and_receive_request("https://api.trafikinfo.trafikverket.se/v2/data.xml", request_body);
    return 0;
}