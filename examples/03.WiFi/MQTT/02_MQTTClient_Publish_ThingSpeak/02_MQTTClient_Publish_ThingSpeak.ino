/*
 * Ejemplo para comunicar MQTTClient con ThingSpeak
 * Basado en: https://www.mathworks.com/help/thingspeak/use-arduino-client-to-publish-to-a-channel.html
 * Require de credenciales para MQTT obtenidas de Devices->MQTT. 
 *  Al descargarlo guardar como mqtt_secrets.h en la misma ubicación que este archivo
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include "mqtt_secrets.h"

#define BUILTIN_LED 2

#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321
// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "My ntwk";
const char* password = "jpim6683";

//MQTT Data
char mqttUserName[] = SECRET_MQTT_USERNAME; // From Devices->MQTT
char mqttPass[] = SECRET_MQTT_PASSWORD;      // From Devices->MQTT
char mqttClientID[] = SECRET_MQTT_CLIENT_ID; // From Devices->MQTT (same as username)
long CHANNEL_ID = 1454821;                  // Cambiar al número de canal .
int fieldNumber = 2;                       // Número usado para publicar en un solo campo
                              
WiFiClient client;   

PubSubClient mqttClient(client); // Inicializa la librería.
const char* server = "mqtt3.thingspeak.com"; 

unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 20L * 1000L; // Publica cada 20 segundos.

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Cambia el estado del LED si se recibe 1 desde un mensaje del tema field3
  if(String(topic).indexOf("field3")!=-1){
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Enciende el LED estableciendo la tension a LOW 
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Apaga el LED estableciendo la tension a HIGH
    }
  }

}

void reconnect()
{

  // Itera hasta reconectarse
  while (!mqttClient.connected()) 
  {
    Serial.print("Intentando una conexion con MQTT...");
    // Se conecta con el MQTT broker
    if (mqttClient.connect(mqttClientID,mqttUserName,mqttPass)) 
    {
      Serial.println("Conectado a MQTT server");
      String message = "channels/";
      message += String(CHANNEL_ID);
      message += "/subscribe/fields/+";
      const char *topicBuffer;
      topicBuffer=message.c_str();
      
      mqttClient.subscribe(topicBuffer);
    } else 
    {
      Serial.print("Fallo de conexion, rc=");
      // Imprime la razón de fallo
      // Visitar https://pubsubclient.knolleary.net/api.html#state para más información.
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

  // Crea una cadena de datos para enviar a ThingSpeak
  String data = String("field1=") + String(t, DEC) + "&field2=" + String(h, DEC);
  const char *msgBuffer;
  msgBuffer=data.c_str();
  Serial.println(msgBuffer);
  
  // Crea una cadena de datos para enviar a ThingSpeak
  String topicString = "channels/" + String( CHANNEL_ID ) + "/publish";
  const char *topicBuffer;
  topicBuffer = topicString.c_str();
  mqttClient.publish( topicBuffer, msgBuffer );
  
  lastConnectionTime =millis(); // actualiza el valor de conexion

}

void mqttPublishField(int field){
  // Lee el valor de la tempertura en Celsius
  float t = dht.readTemperature();
  
  // Crea una cadena de datos para enviar a ThingSpeak
  String data = String(t, DEC);
  const char *msgBuffer;
  msgBuffer=data.c_str();
  Serial.println(msgBuffer);
  
  // Crea una cadena de datos para enviar a ThingSpeak en un campo específico
  String topicString ="channels/" + String( CHANNEL_ID ) + "/publish/fields/field" + String(field);
  const char *topicBuffer;
  topicBuffer = topicString.c_str();
  mqttClient.publish( topicBuffer, msgBuffer );
  
  lastConnectionTime =millis(); // actualiza el valor de conexion

}

void setup()
{
  pinMode(BUILTIN_LED,OUTPUT);
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
  mqttClient.setServer(server, 1883);   // Set the MQTT broker details.
  mqttClient.setCallback(callback);
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

    // si el tiempo ha superado el periodo de publicacion, publicar data en ThingSpeak.
    if (millis() - lastConnectionTime > postingInterval) 
    {
      mqttPublishFeed(); // Publica los valores simultáneamente .
      // mqttPublishField(fieldNumber); // usar esta función para publicar en un único campo
    }
  }
}
