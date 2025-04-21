#ifndef API_REQUEST_TEMPLATES_H
#define API_REQUEST_TEMPLATES_H

#include <stdio.h>
#include <stdlib.h>

#define API_URL "https://api.trafikinfo.trafikverket.se/v2/data.xml"
#define REQUEST_SIZE 512

// Request template for getting train station information
static char* StationInfo(char* api_key, char* locationName) {
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
        "</QUERY>"
        "</REQUEST>",
        api_key, locationName);
    
    return request;
}

#endif // API_REQUEST_TEMPLATES_H