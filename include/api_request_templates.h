#ifndef API_REQUEST_TEMPLATES_H
#define API_REQUEST_TEMPLATES_H

#define API_URL "https://api.trafikinfo.trafikverket.se/v2/data.xml"
#define REQUEST_SIZE 512

// Request template for getting train station information
char* TrainStation(const char* api_key, const char* locationName);
char* TrainAnnouncement(const char* api_key, const char* locationSignature);

#endif // API_REQUEST_TEMPLATES_H