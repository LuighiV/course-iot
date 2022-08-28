/*
 * Ejemplo para conectar el ESP32 a un access point 
 * 
 */

#include <WiFi.h>

char ssid[] = "EKV_MOS";
char pass[] = "34042152";

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
  Serial.print("Conectado :);");
}

void loop() {


  Serial.println(WiFi.localIP());
  delay(1000);
}
