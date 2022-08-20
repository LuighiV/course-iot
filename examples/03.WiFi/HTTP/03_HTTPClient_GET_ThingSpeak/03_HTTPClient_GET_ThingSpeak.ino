/*
 * Ejemplo para crear un cliente HTTP y hacer peticiones con GET desde ThingSpea
 * Basado en https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=http#request-the-data
 *    https://community.thingspeak.com/forum/esp8266-wi-fi/reading-back-data-from-thingspeak/
 *    https://www.mathworks.com/help/thingspeak/readlastentry.html
 */

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* server = "api.thingspeak.com";
const char* CHANNEL_ID = "1448361";

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}


void loop()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    String getStr = "/channels/";
    getStr += CHANNEL_ID;
    getStr += "/feeds/last.json";
    // para valores en canales 
    //getStr += "/fields/2/last.txt";

    if (http.begin(server, 80, getStr)) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  delay(20000);
}
