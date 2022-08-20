/*
 * Ejemplo para crear un cliente HTTP y hacer peticiones con GET desde ThingSpeak
 * Adicionalmente obteniendo datos desde json
 * Basado en https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=http#request-the-data
 * https://www.instructables.com/id/ESP8266-Parsing-JSON/
 * https://arduinojson.org/v6/assistant/
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* server = "api.thingspeak.com";
const char* CHANNEL_ID = "871290";

const size_t capacity = JSON_OBJECT_SIZE(4) + 100;
DynamicJsonDocument doc(capacity);

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
          
          deserializeJson(doc,payload);
          
          const char* created_at = doc["created_at"]; // "2019-09-30T15:08:20Z"
          int entry_id = doc["entry_id"]; // 842
          const char* field1 = doc["field1"]; // "22.6000003815"
          const char* field2 = doc["field2"]; // "70.9000015259"

          //Datos obtenidos
          Serial.print("Creado: ");
          Serial.println(created_at);
          
          Serial.print("Numero de entrada: ");
          Serial.println(entry_id);

          Serial.print("Canal 1: ");
          Serial.println(field1);

          Serial.print("Canal 2: ");
          Serial.println(field2);
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
