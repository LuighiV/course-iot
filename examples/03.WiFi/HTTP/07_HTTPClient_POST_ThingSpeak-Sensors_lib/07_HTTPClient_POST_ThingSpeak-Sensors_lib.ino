/*
 * Ejemplo para crear un cliente HTTP y enviar datos con POST a ThingSpeak
 * Basado en: https://circuits4you.com/2018/03/10/esp8266-nodemcu-post-request-data-to-website/
 */

#include "DHT.h"

#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 4
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321


String API_KEY = "29BIBWZEARZDPILW";  // replace with your channel’s thingspeak API key,

const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* server = "api.thingspeak.com";

// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

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

  dht.begin();
  delay(2000);
  
}


void loop()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    // Obteniendo la informacion de la humedad y temperatura
    // Lee la humedad
    float h = dht.readHumidity();
    // Lee el valor de la tempertura en Celsius
    float t = dht.readTemperature();
  
    // Verifica si alguna lectura es erronea y de serlo lee nuevamente.
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Fallo en lectura del sensor DHT!"));
      return;
    }
  
    Serial.print(F("Humedad: "));
    Serial.print(h);
    Serial.print(F("% \tTemperatura: "));
    Serial.print(t);
    Serial.println(F("°C"));
    
    
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    String postStr = "/update.json";
    String postData = "api_key=";
    postData += API_KEY;
    postData +="&field1=";
    postData += String(t);
    postData +="&field2=";
    postData += String(h);
    
    if (http.begin(server, 80, postStr)) {  // HTTP

      Serial.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      http.addHeader("Content-Type","application/x-www-form-urlencoded");
      int httpCode = http.POST(postData);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  delay(20000);
}
