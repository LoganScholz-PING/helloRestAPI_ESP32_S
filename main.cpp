/*
  Logan Scholz, PING US
  11/23/2021
*/

#include <Arduino.h>
#include "esp_wpa2.h" // for connecting to enterprise
#include <WiFi.h> // important NOT to use the default Arduino library
#include <HTTPClient.h>
#include "WifiConfig.h" // for password and SSID

#define SIZE_OF_MAC_ADDRESS 6 // 6 bytes

HTTPClient http;
WiFiClient wificlient;


/*************************************************************
* Connecting to PING REST API
* Need the following headers:
    
  -Organization:US
  -Content-Type:application/json
  -Ocp-Apim-Subscription-Key:c480f6e38c64410199d9e003baf7ef24  

* Need the following params:

  -workordernumber
*************************************************************/
void connectToInternetHTTP()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    const char* conn = "Connectingto:";
    Serial.println(conn);
    
    String workorder_num = "9267166"; // eventually this will be read out of an RFID tag(??)

    String FULL_API_PATH = String(PING_REST_API_HOST) + url + workorder_num;

    Serial.println(FULL_API_PATH);

    http.begin(FULL_API_PATH);

    http.addHeader("Authorization", "Bearer " + String(PING_JWT_AUTH));
    http.addHeader("Organization", PING_ORG);
    http.addHeader("Content-Type", PING_CONTENT_TYPE);
    http.addHeader("Ocp-Apim-Subscription-Key", PING_OCP_AUTH);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("    * httpCode: " + httpCode);
      Serial.println("    *  payload: " + payload);
    }
    else
    {
      Serial.println("[ERROR] Error on HTTP request");
    }

    http.end();
    }
    else
    {
      Serial.println("[ERROR] Attempt GET but not connected to WiFi!");
    }
}


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

  Serial.print("WiFi Scan Complete. # Networks found: ");
  Serial.println(n);

/* WIFI AUTH ENUMS
    WIFI_AUTH_OPEN = 0,     
    WIFI_AUTH_WEP = 1,            
    WIFI_AUTH_WPA_PSK = 2,         
    WIFI_AUTH_WPA2_PSK = 3,      
    WIFI_AUTH_WPA_WPA2_PSK = 4,     
    WIFI_AUTH_WPA2_ENTERPRISE = 5, 
    WIFI_AUTH_MAX = 6
*/


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

void printMACAddress()
{
  WiFi.disconnect();
  Serial.print(F("Retrieving ESP32 MAC Address....\0"));
  Serial.print(WiFi.macAddress());
  Serial.println("");
  Serial.flush();
  
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
  Serial.print("Connecting-to: ");
  Serial.println(SSID);

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
        Serial.println("WiFi.status() == WL_NO_SHIELD");
        break;
      case WL_IDLE_STATUS:
        Serial.println("WiFi.status() == WL_IDLE_STATUS");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("WiFi.status() == WL_NO_SSID_AVAIL");
        error_count += 1;
        if(error_count >= 10)
        {
          Serial.println("[ERROR] 10 FAILURES. ABORTING CONNECTION ATTEMPT");
          return;
        }
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("WiFi.status() == WL_SCAN_COMPLETED");
        break;
      case WL_CONNECTED:
        Serial.println("WiFi.status() == 1WL_CONNECTED");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("WiFi.status() == 2WL_CONNECT_FAILED");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("WiFi.status() == 3WL_CONNECTION_LOST");
        break;
      case WL_DISCONNECTED:
        Serial.println("WiFi.status() == 4WL_DISCONNECTED");
        break;
      default:
        Serial.print("Unable to interpret WiFi.status() [");
        Serial.print(WiFi.status());
        Serial.println("]");
        break;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() 
{
  Serial.begin(115200);
  delay(10); // settle serial

  Serial.println("***********PRINT MAC ADDRESS************");
  printMACAddress();
  delay(500);
  Serial.println("*********PRINT SCAN NETWORK INFO********");
  WiFi.disconnect(true);
  printScanNetworkInfo();
  delay(500);
  Serial.println("*************CONNECT TO WIFI************");
  connectToWiFi();
  Serial.println("****************************************");
}


void loop() 
{
  //connectToInternetWiFi(); // works to query "Arduino" on google.com
  Serial.println("****************************************");
  Serial.println("Attempting connection to REST API....");
  connectToInternetHTTP();
  delay(10000);
}