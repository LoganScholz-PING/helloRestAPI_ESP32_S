# helloRestAPI_ESP32_S
Navigating ESP32 through WPA2 Enterprise Network Connection + REST API


Important note:
** WiFiConfig.h is a custom library I created to hold security-sensitive credentials and information. The extract below defines the internals of WiFiConfig.h with sensitive info removed:

***************************** START WiFiConfig.h Below *****************************


#ifndef WIFI_CONFIG

#define WIFI_CONFIG


const char* PING_REST_API_HOST = "<RESTAPIPATHHERE>";
  

const char* SSID = "<PINGWIFISSIDHERE>";
  
const char* EAP_ID = "<NETWORKLOGONNAMEHERE>";
  
const char* EAP_USERNAME = "<SAMEASEAP_ID(???)>";
  

const char* PING_JWT_AUTH = "<JWTAUTHTOKENHERE>";
  
const char* PING_OCP_AUTH = "<OCPTOKENHERE>";
  
const char* PING_ORG = "US";
  
const char* PING_CONTENT_TYPE = "application/json";
  
  
String url = "<RESTAPIURLHERE>";
  
  
const char* EAP_PASSWORD = "<YOURPASSWORDHERE>";
  
  
#endif
  

********************************* END WiFiConfig.h ********************************
