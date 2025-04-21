#include <stdio.h>
#include <stdlib.h>
#include <api_request_templates.h>

char* TrainStation(const char* api_key, const char* locationName) {
    char* request = malloc(REQUEST_SIZE); // Allocate memory for the request string
    if (request == NULL) {
        perror("Not enough memory (malloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }
    
    snprintf(request, REQUEST_SIZE,
        "<REQUEST>"
        "<LOGIN authenticationkey='%s'/>"
        "<QUERY objecttype='TrainStation' namespace='rail.infrastructure' schemaversion='1.5' limit='10'>"
        "<FILTER>"
        "<EQ name='AdvertisedLocationName' value='%s'/>"
        "</FILTER>"
        "<EXCLUDE>Deleted</EXCLUDE>"
        "<EXCLUDE>AdvertisedShortLocationName</EXCLUDE>"
        "<EXCLUDE>Advertised</EXCLUDE>"
        "</QUERY>"
        "</REQUEST>",
        api_key, locationName);
    
    return request;
}

char* TrainAnnouncement(const char* api_key, const char* locationSignature) {
    char* request = malloc(REQUEST_SIZE); // Allocate memory for the request string
    if (request == NULL) {
        perror("Not enough memory (malloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    snprintf(request, REQUEST_SIZE,
        "<REQUEST>"
        "<LOGIN authenticationkey='%s'/>"
        "<QUERY objecttype='TrainAnnouncement' schemaversion='1.9' limit='10'>"
        "<FILTER>"
        "<EQ name='LocationSignature' value='%s'/>"
        "</FILTER>"
        "</QUERY>"
        "</REQUEST>",
        api_key, locationSignature);
    
    return request;
}
