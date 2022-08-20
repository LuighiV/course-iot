/*
 * Ejemplo para crear un cliente HTTP y hacer peticiones con GET desde ThingSpeak actualizando datos
 * Basado en https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=http#request-the-data
 *    https://community.thingspeak.com/forum/esp8266-wi-fi/reading-back-data-from-thingspeak/
 *    https://www.mathworks.com/help/thingspeak/writedata.html
 */

#include <WiFi.h>
#include <HTTPClient.h>

#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321

const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* server = "api.thingspeak.com";

String API_KEY = "29BIBWZEARZDPILW";  // replace with your channel’s thingspeak API key,


// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

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

    String getStr = "/update.json?api_key=";
    getStr += API_KEY;
    getStr +="&field1=";
    getStr += String(t);
    getStr +="&field2=";
    getStr += String(h);

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
