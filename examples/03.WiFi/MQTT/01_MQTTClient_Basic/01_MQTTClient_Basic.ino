/*
 * Ejemplo para comunicar cliente MQTT con servidor gratuito
 * Basado en ejemplo mqtt_esp8266
*/

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MyWifi";
const char* password = "jpim6683";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define BUILTIN_LED 2

void setup_wifi() {

  delay(10);
  // Conectarse a la red Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Encender el LED si recibe como primer caracter a 1
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Enciende el LED estableciendo la tensión en LOW
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Apaga el LED estableciendo la tensión en HIGH
  }

}

void reconnect() {
  // Itera hasta reconectarse
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Crea un cliente aleatorio
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Intenta conectarse
    if (client.connect(clientId.c_str())) {
      Serial.print("connected with Client ID: ");
      Serial.println(clientId);
      // Una vez conectado publica un anuncio
      client.publish("ESP32/Desde-ESP32","Hola, soy ESP32");
      // y se suscribe a un tópico
      client.subscribe("ESP32/Hacia-ESP32");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // espera 5 segundos antes de intentar reconectarse
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Inicializa el LED_BUILTIN como salida
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("ESP32/Desde-ESP32", msg);
  }
}
