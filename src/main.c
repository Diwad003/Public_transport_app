#include <https_post.h>
#include <api_request_templates.h>
#include <stdlib.h>

void destroy_xml_data(struct xml_info* xml_data, int xml_data_size) {
    if (xml_data == NULL) {
        return;
    }
    for (int j = 0; j < xml_data_size; j++) {
        free(xml_data[j].post);
        free(xml_data[j].value);
    }
    free(xml_data);
}

int main(int argc, char *argv[]) {
    struct xml_info* xml_data = NULL;
    int xml_data_size = 0;
    char* request = NULL;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <API_KEY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    request = StationInfo(argv[1], "Lund C");
    xml_data = extract_xml_values(send_and_receive_request(API_URL,request), &xml_data_size);
    
    for (int j = 0; j < xml_data_size; j++) {
        fprintf(stderr, "port:%s\n", xml_data[j].post);
        fprintf(stderr, "value:%s\n", xml_data[j].value);
    }

    // Free allocated memory
    destroy_xml_data(xml_data, xml_data_size);
    return 0;
}