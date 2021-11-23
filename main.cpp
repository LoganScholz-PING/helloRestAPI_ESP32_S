/*
  Logan Scholz, PING US
  11/23/2021
*/

#include <Arduino.h>
#include "esp_wpa2.h" // for connecting to enterprise
#include <WiFi.h> // important NOT to use the default Arduino library
#include <HTTPClient.h>
#include "WifiConfig.h" // for password and SSID
#include "ArduinoJson.h"

#define SIZE_OF_MAC_ADDRESS 6 // 6 bytes

HTTPClient http;
WiFiClient wificlient;

//StaticJsonDocument<12288> so_json; // byte size calculated from https://arduinojson.org/v6/assistant/
StaticJsonDocument<768> wo_json; // byte size calculated from https://arduinojson.org/v6/assistant/

bool debug = false; // control print statements


/*************************************************************
* Connecting to PING REST API
* Need the following headers:
    
  - Organization:US
  - Content-Type:application/json
  - Ocp-Apim-Subscription-Key:c480f6e38c64410199d9e003baf7ef24  

* Need the following params:

  -workordernumber
*************************************************************/
void connectToInternetHTTP()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Connecting to: ");
    
    char workorder_num[] = "9267166"; // eventually this will be read out of an RFID tag(??)

    String FULL_API_PATH = String(PING_REST_API_HOST) + wo_url + workorder_num;

    Serial.println(FULL_API_PATH);

    http.begin(FULL_API_PATH);

    http.addHeader("Authorization", "Bearer " + String(PING_JWT_AUTH));
    http.addHeader("Organization", PING_ORG);
    http.addHeader("Content-Type", PING_CONTENT_TYPE);
    http.addHeader("Ocp-Apim-Subscription-Key", PING_OCP_AUTH);

    int httpCode = http.GET(); // i'm pretty sure this line is blocking. There is an Async method I have not tried yet

    // TODO:
    // create a JSON object to hold the data returned by wificlient.readStringUntil('\r');
    // then deserialize this object and print the following:
    //    masterWorkOrderNumber
    //    orderNumber
    //    itemNumber

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println();
      Serial.println("*** http.GET() DETAILS ***");
      if(debug) Serial.println("  * httpCode: " + String(httpCode));
      Serial.println("  *  payload: ");
      Serial.println(payload);
      Serial.println("****** ATTEMPT JSON PARSE BELOW ******");

      DeserializationError DE = deserializeJson(wo_json, payload);

      if (DE)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(DE.f_str());
        return;
      }

      int wo_num = wo_json["workOrderNumber"];
      int master_wo_num = wo_json["masterWorkOrderNumber"];
      int order_num = wo_json["orderNumber"];
      double item_num = wo_json["itemNumber"];
      Serial.print("workOrderNumber: "); 
      Serial.println(wo_num);
      Serial.print("masterWorkOrderNumber: ");
      Serial.println(master_wo_num);
      Serial.print("orderNumber: ");
      Serial.println(order_num);
      Serial.print("itemNumber: ");
      Serial.println(item_num, 2);
    }
    else
    {
      // likely internal error to ESP32
      Serial.println("[ERROR] Error on HTTP request");
    }

    http.end();
  }
  else
  {
    Serial.println("[ERROR] Attempt GET but not connected to WiFi!");
  }
}

// connectToInternetWiFi() just searches google.com for "arduino" and prints the returned results.
// purely for testing and evaluation
void connectToInternetWiFi()
{
  const char* host = "google.com"; // for testing

  Serial.print("Connecting to host ");
  Serial.println(host);


  //if (!wificlient.connect(host, 80)) // for testing
  if (!wificlient.connect(host, 80))
  {
    Serial.println("[ERROR] Connection to host failed");
    return;
  }

  // url = universal resource locator
  String url = "/search?q=arduino HTTP/1.0"; // for testing
  

  Serial.print("Requesting URL: ");
  Serial.println(url);

  //wificlient.print(String("GET ") + url + "\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  wificlient.println("GET " + url);
  wificlient.println();

  unsigned long timeout = millis();

  while (wificlient.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> ERROR, Client Timeout!");
      wificlient.stop();
      return;
    }
  }

  while(wificlient.available())
  {
    String line = wificlient.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("[INFO] Closing Connection to Host");
}

void printScanNetworkInfo()
{
  int n = WiFi.scanNetworks(); // i think when you execute this line, WiFi.SSID[] is loaded with current info

  if (n == 0)
  {
    Serial.println("No networks found!");
    return;
  }

  if(debug) Serial.print("WiFi Scan Complete. # Networks found: ");
  if(debug) Serial.println(n);

  /*********************************
  WIFI AUTH ENUMS
    WIFI_AUTH_OPEN = 0,     
    WIFI_AUTH_WEP = 1,            
    WIFI_AUTH_WPA_PSK = 2,         
    WIFI_AUTH_WPA2_PSK = 3,      
    WIFI_AUTH_WPA_WPA2_PSK = 4,     
    WIFI_AUTH_WPA2_ENTERPRISE = 5, 
    WIFI_AUTH_MAX = 6
  *********************************/


  if(debug) 
  {
    for (int i = 0; i < n; ++i)
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i)); // received signal strength indicator
      Serial.print(")");
      Serial.print( (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "-OPEN" : "-SECURED" );
      Serial.print(" [");
      Serial.print(WiFi.encryptionType(i));
      Serial.println("]");
      delay(10);
    }
  }
}

void printMACAddress()
{
  WiFi.disconnect();

  if(debug) 
  {
    Serial.print(F("Retrieving ESP32 MAC Address... "));
    Serial.print(WiFi.macAddress());
    Serial.println("");
    Serial.flush();
  }
  
  //Serial.print("Easy MAC Address: ");
  //Serial.println(WiFi.macAddress()); // easy way to get MAC ID

  //Serial.println("Hard MAC Address: ");
  // uint8_t mac_addr[SIZE_OF_MAC_ADDRESS];
  // WiFi.macAddress(mac_addr);

  // for (byte i = 0; i < SIZE_OF_MAC_ADDRESS; ++i)
  // {
  //   Serial.print(mac_addr[i], HEX);
  //   if(i < SIZE_OF_MAC_ADDRESS - 1) Serial.print(":");
  // }

  //Serial.println();
}


void connectToWiFi()
{
  // char* test1 = "Connecting-to- ";
  // byte size = sizeof(test1) + sizeof(SSID);
  // char buff[size];
  // strcpy(buff, test1);
  // strcat(buff, SSID);
  // Serial.println(buff);
  if(debug) Serial.print("Connecting to: ");
  if(debug) Serial.println(SSID);

  // start WPA2 enterprise config magic
  WiFi.disconnect(true);

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t*)EAP_ID,       strlen(EAP_ID)      );
  esp_wifi_sta_wpa2_ent_set_username((uint8_t*)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t*)EAP_PASSWORD, strlen(EAP_PASSWORD));

  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&config);
  // end WPA2 enterprise config magic
  
  WiFi.begin(SSID);

  /* WiFi.status() enum definitions:
      WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
      WL_IDLE_STATUS      = 0,
      WL_NO_SSID_AVAIL    = 1,
      WL_SCAN_COMPLETED   = 2,
      WL_CONNECTED        = 3,
      WL_CONNECT_FAILED   = 4,
      WL_CONNECTION_LOST  = 5,
      WL_DISCONNECTED     = 6
  */

  int error_count = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    // more verbose print statements for the current status of the connection
    
    switch( WiFi.status() )
    {
      case WL_NO_SHIELD:
        if(debug) Serial.println("WiFi.status() == WL_NO_SHIELD");
        break;
      case WL_IDLE_STATUS:
        if(debug) Serial.println("WiFi.status() == WL_IDLE_STATUS");
        break;
      case WL_NO_SSID_AVAIL:
        if(debug) Serial.println("WiFi.status() == WL_NO_SSID_AVAIL");
        error_count += 1;
        if(error_count >= 10)
        {
          Serial.println("[ERROR] 10 FAILURES. ABORTING CONNECTION ATTEMPT");
          return;
        }
        break;
      case WL_SCAN_COMPLETED:
        if(debug) Serial.println("WiFi.status() == WL_SCAN_COMPLETED");
        break;
      case WL_CONNECTED:
        if(debug) Serial.println("WiFi.status() == WL_CONNECTED");
        break;
      case WL_CONNECT_FAILED:
        if(debug) Serial.println("WiFi.status() == WL_CONNECT_FAILED");
        break;
      case WL_CONNECTION_LOST:
        if(debug) Serial.println("WiFi.status() == WL_CONNECTION_LOST");
        break;
      case WL_DISCONNECTED:
        if(debug) Serial.println("WiFi.status() == WL_DISCONNECTED");
        break;
      default:
        Serial.print("Unable to interpret WiFi.status() [");
        Serial.print(WiFi.status());
        Serial.println("]");
        break;
    }
  }

  Serial.println("");
  Serial.print("Connected to " + String(SSID) + ". IP Address: " + String(WiFi.localIP()));
}

void setup() 
{
  Serial.begin(115200);
  delay(10); // settle serial

  if(debug) Serial.println("***********PRINT MAC ADDRESS************");
  printMACAddress();
  delay(500);
  if(debug) Serial.println("*********PRINT SCAN NETWORK INFO********");
  WiFi.disconnect(true);
  printScanNetworkInfo();
  delay(500);
  if(debug) Serial.println("*************CONNECT TO WIFI************");
  connectToWiFi();
  if(debug) Serial.println("****************************************");
}


void loop() 
{
  //connectToInternetWiFi(); // works to query "Arduino" on google.com
  Serial.println("****************************************");
  Serial.println("Attempting connection to REST API....");
  connectToInternetHTTP();
  delay(10000);
}