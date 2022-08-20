/*
 * Ejemplo para crear un cliente HTTP y controlar LED desde ThingSpea
 * Basado en https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=http#request-the-data
 *    https://community.thingspeak.com/forum/esp8266-wi-fi/reading-back-data-from-thingspeak/
 *    https://www.mathworks.com/help/thingspeak/readlastentry.html
 * Para escribir en el canal y controlar el LED:
 * Apagar
 *    GET https://api.thingspeak.com/update?api_key=3CYVQIXIEQAOIINF&field1=0
 * Encender
 *    GET https://api.thingspeak.com/update?api_key=3CYVQIXIEQAOIINF&field1=1
 */

#include <WiFi.h>
#include <HTTPClient.h>

const int LED = 2;
const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* server = "api.thingspeak.com";
const char* CHANNEL_ID = "1454821";
const char* API_KEY = "Z52BPQ9LYOQT3CAN";

int state = 0;

void setup()
{
  pinMode(LED,OUTPUT);
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
    // para valores en canales 
    getStr += "/fields/3/last.txt";
    getStr += "?api_key=";
    getStr += API_KEY;

    Serial.println(getStr);

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
          state = payload.toInt()<= 0 ? LOW : HIGH;
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  digitalWrite(LED,state);
  delay(1000);
}
