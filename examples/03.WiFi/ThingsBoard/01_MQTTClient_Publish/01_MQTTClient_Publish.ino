/*
 * Ejemplo para comunicar con ThingsBoard using generic libraries
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"

#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321
// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

#define LED_BUILTIN 2

const char* ssid = "EKV_MOS";
const char* password = "34042152";

DynamicJsonDocument doc(200);

//MQTT Data
char mqttUserName[] = "DHT11_DEMO_TOKEN";    // Cambiar a access Token
char topic[] = "v1/devices/me/telemetry";

static const char alphanum[] ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz";  // Para generación aleatoria de client ID.
                              
WiFiClient client;   

PubSubClient mqttClient(client); // Inializa la librería.
const char* server = "thingsboard.iot.projects.luighiviton.com"; 

unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 5L * 1000L; // Publica datos cada 5 segundos.

void reconnect()
{
  char clientID[9];
  
  // Itera hasta reconectarse
  while (!mqttClient.connected()) 
  {
    Serial.print("Intentando una conexion con MQTT...");
    // Genera ClientID
    for (int i = 0; i < 8; i++) {
        clientID[i] = alphanum[random(51)];
    }
    clientID[8]='\0';

    // Se conecta con broker MQTT.
    if (mqttClient.connect(clientID,mqttUserName,"")) 
    {
      Serial.println("Conectado a MQTT server");

      String topic = "v1/devices/me/telemetry";
      mqttClient.subscribe(topic.c_str());
    } else 
    {
      Serial.print("Fallo de conexion, rc=");
      // Imprime la razón si ha fallado la conexión
      // See https://pubsubclient.knolleary.net/api.html#state para explicación de fallo.
      Serial.print(mqttClient.state());
      Serial.println(" intentar nuevamente en 5 segundos");
      delay(5000);
    }
  }
}

void mqttPublishFeed(){
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

  char h_char[10];
  sprintf(h_char, "%.2f", h);
  char t_char[10];
  sprintf(t_char, "%.2f", t);

  doc["temperature"] = t;
  doc["humidity"] = h;
  String buffer;
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer.c_str());
  
  lastConnectionTime =millis(); // actualiza el valor de conexion

}


void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  
  Serial.begin(115200);
  Serial.println();
  
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected to Wi-Fi");
  mqttClient.setServer(server, 1883);   // Establece los parámetros de servidor MQTT.
  dht.begin();
  delay(2000);
}


void loop()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    //reconectar si el cliente se ha desconectado
    if(!mqttClient.connected())
    {
      reconnect();
    }

    mqttClient.loop();   // establece una conexion continua con el servidor

    // si el tiempo ha superado el periodo de publicacion, publicar data en Adafruit
    if (millis() - lastConnectionTime > postingInterval) 
    {
      mqttPublishFeed(); // Publica los valores en Adafruit
    }
  }
}
