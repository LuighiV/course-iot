/*
 * Ejemplo para comunicar MQTTClient con Adafruit
 * Basado en: https://www.mathworks.com/help/thingspeak/use-arduino-client-to-publish-to-a-channel.html
 */

#include <WiFi.h>
#include <PubSubClient.h>

#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321
// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

#define LED_BUILTIN 2

const char* ssid = "MyWifi";
const char* password = "jpim6683";

//MQTT Data
char mqttUserName[] = "LuighiViton";                        // Cambiar a nombre de usuario de Adafruit
char mqttPass[] = "9517032f6ed44e7fa082a1ded5333fa4";      // Cambiar a MQTT Key en AIO Key de Adafruit.   

static const char alphanum[] ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz";  // Para generación aleatoria de client ID.
                              
WiFiClient client;   

PubSubClient mqttClient(client); // Inializa la librería.
const char* server = "io.adafruit.com"; 

unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 5L * 1000L; // Publica datos cada 5 segundos.

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String str_payload = "";
  for (int i = 0; i < length; i++) {
    str_payload += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if(String(topic).indexOf("led-control")!=-1){
    if(str_payload.indexOf("ON") != -1){
      digitalWrite(LED_BUILTIN,LOW);
    }else{
      digitalWrite(LED_BUILTIN,HIGH);
    }
  }
}

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
    if (mqttClient.connect(clientID,mqttUserName,mqttPass)) 
    {
      Serial.println("Conectado a MQTT server");

      String topic = String(mqttUserName)+ "/feeds/course-iot.led-control";
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

  String topic1= String(mqttUserName)+ "/feeds/course-iot.humedad";
  mqttClient.publish(topic1.c_str(), h_char);
  
  String topic2= String(mqttUserName)+ "/feeds/course-iot.temperatura";
  mqttClient.publish(topic2.c_str(), t_char);

  
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

    // si el tiempo ha superado el periodo de publicacion, publicar data en Adafruit
    if (millis() - lastConnectionTime > postingInterval) 
    {
      mqttPublishFeed(); // Publica los valores en Adafruit
    }
  }
}
