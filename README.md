# helloRestAPI_ESP32_S
Navigating ESP32 through WPA2 Enterprise Network Connection + REST API


*************************************************
Necessary Libraries:
Arduino JSON - https://arduinojson.org/v6/doc/
*************************************************


Important note:
** WiFiConfig.h is a custom library I created to hold security-sensitive credentials and information. The extract below defines the internals of WiFiConfig.h with sensitive info removed:

***************************** START WiFiConfig.h Below *****************************


#ifndef WIFI_CONFIG

#define WIFI_CONFIG


const char* PING_REST_API_HOST = "REST API PATH HERE";
  

const char* SSID = "PING WIFI SSID HERE";
  
const char* EAP_ID = "NETWORK LOGON NAME HERE";
  
const char* EAP_USERNAME = "SAME AS EAP_ID (???)";
  

const char* PING_JWT_AUTH = "JWT AUTH TOKEN HERE";
  
const char* PING_OCP_AUTH = "OCP TOKEN HERE";
  
const char* PING_ORG = "US";
  
const char* PING_CONTENT_TYPE = "application/json";
  
  
String wo_url = "REST API WORKORDER URL HERE";
  
  
const char* EAP_PASSWORD = "YOUR PASSWORD HERE";
  
  
#endif
  

********************************* END WiFiConfig.h ********************************
