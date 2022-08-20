/*
 * Ejemplo para conectar el ESP32 a un access point 
 * 
 */

#include <WiFi.h>

char ssid[] = "LV_WLAN";
char pass[] = "IoTKey_12%89*";

void setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, pass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  Serial.print("Conectado :);");
  Serial.println(WiFi.localIP());
}
