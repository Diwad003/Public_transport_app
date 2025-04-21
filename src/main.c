#include <https_post.h>
#include <api_request_templates.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cjson/cJSON.h>
 

struct deviation {
    char* code;
    char* description;
};

struct from_location {
    char* location_name;
    int order;
    int priority;
};

struct train_information {
    char* activity_id;
    char* activity_type;
    char* advertised_time_at_location;
    char* advertised_Train_Identifier;
    bool canceled;
    struct deviation* deviation;
    char* estimated_time_at_location;
    bool estimated_time_is_preliminary;
    struct from_location* from_location;
    char* information_owner;
    char* location_date_OTN;
    char* location_signature;
    char* operator;
    char* track;
    struct from_location* via_to_location;


    struct train_information* next;
    struct train_information* prev;
};

void print_json_data(const char* json_string, FILE* fp) {
    if (json_string == NULL) {
        fprintf(stderr, "No JSON data received.\n");
        return;
    }
    if (fp == NULL) {
        perror("Failed to open log.txt for writing.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "JSON Response: %s\n", json_string);
}


struct train_information* parse_json(const char* json_string, struct train_information* train_data_head) {
    struct train_information* train_data = NULL;
    
    // Parse the JSON string
    cJSON* root = cJSON_Parse(json_string);
        if (!root) {
            fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
            return NULL;
        }
    
        // Navigate to the TrainAnnouncement array
        cJSON* response = cJSON_GetObjectItem(root, "RESPONSE");
        if (!response) {
            fprintf(stderr, "Error: RESPONSE not found in JSON.\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        cJSON* result = cJSON_GetObjectItem(response, "RESULT");
        if (!result || !cJSON_IsArray(result)) {
            fprintf(stderr, "Error: RESULT not found or is not an array.\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // Iterate through the RESULT array
        cJSON* result_item = NULL;
        cJSON_ArrayForEach(result_item, result) {
            cJSON* train_announcement = cJSON_GetObjectItem(result_item, "TrainAnnouncement");
            if (!train_announcement || !cJSON_IsArray(train_announcement)) {
                continue;
            }
    
            // Iterate through the TrainAnnouncement array
            cJSON* announcement = NULL;
            cJSON_ArrayForEach(announcement, train_announcement) {

                // Allocate memory for the train_data array
                train_data = malloc(sizeof(struct train_information));
                if (train_data == NULL) {
                    perror("Not enough memory (malloc returned NULL)\n");
                    exit(EXIT_FAILURE);
                }
                memset(train_data, 0, sizeof(struct train_information)); // Initialize all fields to NULL/0
                train_data->next = NULL;
                train_data->prev = NULL;

                // Extract fields from the JSON
                cJSON* activity_id = cJSON_GetObjectItem(announcement, "ActivityId");
                if (activity_id && cJSON_IsString(activity_id)) {
                    train_data->activity_id = strdup(activity_id->valuestring);
                }

                cJSON* activity_type = cJSON_GetObjectItem(announcement, "ActivityType");
                if (activity_type && cJSON_IsString(activity_type)) {
                    train_data->activity_type = strdup(activity_type->valuestring);
                }

                cJSON* advertised_time_at_location = cJSON_GetObjectItem(announcement, "AdvertisedTimeAtLocation");
                if (advertised_time_at_location && cJSON_IsString(advertised_time_at_location)) {
                    train_data->advertised_time_at_location = strdup(advertised_time_at_location->valuestring);
                }

                cJSON* advertised_train_identifier = cJSON_GetObjectItem(announcement, "AdvertisedTrainIdent");
                if (advertised_train_identifier && cJSON_IsString(advertised_train_identifier)) {
                    train_data->advertised_Train_Identifier = strdup(advertised_train_identifier->valuestring);
                }

                cJSON* canceled = cJSON_GetObjectItem(announcement, "Canceled");
                if (canceled && cJSON_IsBool(canceled)) {
                    train_data->canceled = cJSON_IsTrue(canceled);
                }

                cJSON* location_signature = cJSON_GetObjectItem(announcement, "LocationSignature");
                if (location_signature && cJSON_IsString(location_signature)) {
                    train_data->location_signature = strdup(location_signature->valuestring);
                }

                cJSON* track = cJSON_GetObjectItem(announcement, "TrackAtLocation");
                if (track && cJSON_IsString(track)) {
                    train_data->track = strdup(track->valuestring);
                }

                cJSON* operator = cJSON_GetObjectItem(announcement, "Operator");
                if (operator && cJSON_IsString(operator)) {
                    train_data->operator = strdup(operator->valuestring);
                }

                cJSON* information_owner = cJSON_GetObjectItem(announcement, "InformationOwner");
                if (information_owner && cJSON_IsString(information_owner)) {
                    train_data->information_owner = strdup(information_owner->valuestring);
                }

                // Extract nested FromLocation array
                cJSON* from_location = cJSON_GetObjectItem(announcement, "FromLocation");
                if (from_location && cJSON_IsArray(from_location)) {
                    cJSON* location = cJSON_GetArrayItem(from_location, 0); // Assuming only one location
                    if (location) {
                        train_data->from_location = malloc(sizeof(struct from_location));
                        if (train_data->from_location == NULL) {
                            perror("Not enough memory (malloc returned NULL)\n");
                            exit(EXIT_FAILURE);
                        }
                        memset(train_data->from_location, 0, sizeof(struct from_location));

                        cJSON* location_name = cJSON_GetObjectItem(location, "LocationName");
                        if (location_name && cJSON_IsString(location_name)) {
                            train_data->from_location->location_name = strdup(location_name->valuestring);
                        }
                        
                        cJSON* order = cJSON_GetObjectItem(location, "Order");
                        if (order && cJSON_IsNumber(order)) {
                            train_data->from_location->order = order->valueint;
                        }

                        cJSON* priority = cJSON_GetObjectItem(location, "Priority");
                        if (priority && cJSON_IsNumber(priority)) {
                            train_data->from_location->priority = priority->valueint;
                        }
                    }
                }

                cJSON* deviation = cJSON_GetObjectItem(announcement, "Deviation");
                if (deviation && cJSON_IsArray(deviation)) {
                    cJSON* location = cJSON_GetArrayItem(deviation, 0); // Assuming only one location
                    if (location) {
                        train_data->deviation = malloc(sizeof(struct deviation));
                        if (train_data->deviation == NULL) {
                            perror("Not enough memory (malloc returned NULL)\n");
                            exit(EXIT_FAILURE);
                        }
                        memset(train_data->deviation, 0, sizeof(struct deviation));

                        cJSON* location_name = cJSON_GetObjectItem(location, "Code");
                        if (location_name && cJSON_IsString(location_name)) {
                            train_data->deviation->code = strdup(location_name->valuestring);
                        }
                        
                        cJSON* order = cJSON_GetObjectItem(location, "Deviation");
                        if (order && cJSON_IsNumber(order)) {
                            train_data->deviation->description = order->valuestring;
                        }
                    }
                }
                
                if (train_data_head == NULL) {
                    // If the list is empty, the new node becomes the head
                    train_data_head = train_data;
                } else {
                    // Find the last node in the list
                    struct train_information* tail = train_data_head;
                    while (tail->next != NULL) {
                        tail = tail->next;
                    }
                    // Append the new node
                    tail->next = train_data;
                    train_data->prev = tail;
                }
            }
        }   
    // Clean up
    cJSON_Delete(root);
    return train_data_head;
}

void print_train_information(struct train_information* head) {
    struct train_information* current = head;

    if (!current) {
        printf("No train information available.\n");
        return;
    }

    while (current != NULL) {
        printf("ActivityId: %s\n", current->activity_id ? current->activity_id : "N/A");
        printf("ActivityType: %s\n", current->activity_type ? current->activity_type : "N/A");
        printf("AdvertisedTimeAtLocation: %s\n", current->advertised_time_at_location ? current->advertised_time_at_location : "N/A");
        printf("AdvertisedTrainIdent: %s\n", current->advertised_Train_Identifier ? current->advertised_Train_Identifier : "N/A");
        printf("Canceled: %s\n", current->canceled ? "true" : "false");
        printf("LocationSignature: %s\n", current->location_signature ? current->location_signature : "N/A");
        printf("TrackAtLocation: %s\n", current->track ? current->track : "N/A");
        printf("Operator: %s\n", current->operator ? current->operator : "N/A");
        printf("InformationOwner: %s\n", current->information_owner ? current->information_owner : "N/A");

        if (current->from_location) {
            printf("FromLocation:\n");
            printf("  LocationName: %s\n", current->from_location->location_name ? current->from_location->location_name : "N/A");
            printf("  Priority: %d\n", current->from_location->priority);
            printf("  Order: %d\n", current->from_location->order);
        } else {
            printf("FromLocation: N/A\n");
        }

        if (current->deviation) {
            printf("Deviation:\n");
            printf("  Code: %s\n", current->deviation->code ? current->deviation->code : "N/A");
            printf("  Description: %s\n", current->deviation->description ? current->deviation->description : "N/A");
        } else {
            printf("Deviation: N/A\n");
        }

        printf("\n");
        current = current->next;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <API_KEY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct train_information* train_data = NULL;
    char* request = NULL;

    request = TrainAnnouncement(argv[1], "Lu");
    char* json_data = send_and_receive_request(API_URL, request);
    train_data = parse_json(json_data, train_data);

    struct train_information* train_data_ptr = train_data;
    print_train_information(train_data_ptr);

    free(request);
    return 0;
}