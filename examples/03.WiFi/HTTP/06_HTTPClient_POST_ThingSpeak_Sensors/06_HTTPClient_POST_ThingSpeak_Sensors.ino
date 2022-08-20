/*
 * Ejemplo para publicar datos de NodeMCU a Thingspeak
 * basado en: https://www.electronicwings.com/nodemcu/http-client-on-nodemcu-with-arduino-ide
 * 
 */

#include "DHT.h"

#include <WiFi.h>

#define DHTPIN 4
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321

String API_KEY = "X20GXQQF174NYMXX";  // replace with your channel’s thingspeak API key,

const char* ssid = "MyWifi";    // Enter SSID here
const char* password = "jpim6683";  // Enter Password here

const char* server = "api.thingspeak.com";

// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(F("Probando el DHT!"));

  dht.begin();
  delay(2000);
}

void loop() 
{
  
  if (client.connect(server,80))     // "184.106.153.149" or api.thingspeak.com
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

    
    String postStr = "api_key=";
    postStr += API_KEY;
    postStr +="&field1=";
    postStr += String(t);
    postStr +="&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";
    Serial.println(postStr);

    Serial.println("[Sending a request]");
    
    client.print("POST /update.json HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+API_KEY+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
    
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }
  delay(20000);
}
