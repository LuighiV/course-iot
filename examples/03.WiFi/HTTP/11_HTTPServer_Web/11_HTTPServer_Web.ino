/*
 * Ejemplo para servidor web empleando librería ESP8266Webserver
 * Basado en ejemplo: HelloServer
 * 
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT21   // DHT 22  (AM2302), AM2321

const char* ssid = "MyWifi";
const char* password = "jpim6683";

const char* DOMAIN_NAME = "esp32";
WebServer server(80);
// Inicializa la libreria
DHT dht(DHTPIN, DHTTYPE);

#define LED_BUILTIN 2
const int led = 2;

String prepareMainPage(){

  float h = dht.readHumidity();
  // Lee el valor de la tempertura en Celsius
  float t = dht.readTemperature();
    
  String path = server.uri();
  int val = 0;
  if (path.indexOf(F("/on")) != -1) {
    val = 1;
  } else if (path.indexOf(F("/off")) != -1) {
    val = 0;
  } else{ 
      if (path == "/"){
        Serial.println(F("Home page"));
      } else{
        Serial.println(F("invalid request"));
      }
    val = digitalRead(led);
  }
  // Set LED according to the request
  digitalWrite(led, val);
  
  String stringValue = val ? ("off"): ("on");
  //String address = String(DOMAIN_NAME);
  String address = WiFi.localIP().toString();
  String order = val ? ("Encendido"): ("Apagado");
  String htmlPage =
       String("<!DOCTYPE HTML>") +
              "<html><body>" +
              "\r\n<h1>Bienvenido a ESP32 page!</h1>"+
              "\r\n A traves de esta pagina se controlan los LEDs y "+
              "se aprecian los valores del ADC y sensores.\n\n"+
              "<h2>GPIO</h2>\n Haz click en el boton para "+
              "cambiar de estado.<br><br> Estado actual: "+
              "\r\n<a href='http://"+
              address+"/"+stringValue+"'><button>"+ order+
              "</button></a>\n"+
              "<h2>ADC</h2>\n Valor del potenciometro:"+
              String(analogRead(A0))+
              "<h2>Sensores</h2>\n Valores de los sensores.<br>"+
              "<b>Humedad </b>"+
              String(h)+
              "<br><b>Temperatura </b>"+
              String(t)+
              "</body></html>" +
              "\r\n";
              
    return htmlPage;
}

void handleRoot() {
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(200, "text/html", prepareMainPage());
  digitalWrite(LED_BUILTIN, LOW);
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, LOW);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(led, LOW);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(DOMAIN_NAME)) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/on", handleRoot);
  server.on("/off", handleRoot);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  dht.begin();
  delay(2000);
  
}

void loop(void) {
  server.handleClient();
  delay(2);
}
