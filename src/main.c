#include <https_post.h>
#include <api_request_templates.h>
#include <stdlib.h>

void destroy_xml_data(struct xml_inforamtion* xml_data, int xml_data_size) {
    if (xml_data == NULL || xml_data_size <= 0) {
        return;
    }
    for (int j = 0; j < xml_data_size; j++) {
        free(xml_data[j].post);
        free(xml_data[j].value);
    }
    free(xml_data);
}

void print_xml_data(struct xml_inforamtion* xml_data, int xml_data_size) {
    if (xml_data == NULL || xml_data_size <= 0) {
        return;
    }
    for (int i = 0; i < xml_data_size; i++) {
        fprintf(stderr, "%s\n", xml_data[i].post);
        fprintf(stderr, "%s\n", xml_data[i].value);
    }
}

int main(int argc, char *argv[]) {
    struct xml_inforamtion* xml_data = NULL;
    int xml_data_size = 0;

    char* request = NULL;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <API_KEY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    request = TrainAnnouncement(argv[1], "Lu");
    xml_data = extract_xml_values(send_and_receive_request(API_URL,request), &xml_data_size);
    print_xml_data(xml_data, xml_data_size);

    destroy_xml_data(xml_data, xml_data_size);
    free(request);
    return 0;
}