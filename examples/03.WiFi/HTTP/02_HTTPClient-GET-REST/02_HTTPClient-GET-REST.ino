/*
 * Ejemplo para crear un cliente HTTP y hacer peticiones con GET con estructura REST
 * Basado en https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=http#request-the-data
 */

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* URL = "http://jsonplaceholder.typicode.com/users/1";

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
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, URL)) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      // inicia la conexion y envia la cabecera HTTP
      int httpCode = http.GET();

      // la respuesta sera negativa en caso de error
      if (httpCode > 0) {
        // se recibe la respuesta desde el servidor
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // si se ha encontrar un archivo en el servidor
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
