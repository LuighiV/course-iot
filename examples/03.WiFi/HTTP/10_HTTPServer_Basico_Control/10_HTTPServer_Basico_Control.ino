/*
 * Ejemplo de un servidor HTTP basico para control de LEDs
 * Basado en: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
 * 
 */

#include <WiFi.h>

const int LED = 2;
int val = 0; // almacena estado del LED

const char* ssid = "MyWifi";
const char* password = "jpim6683";

WiFiServer server(80);

void setup()
{
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  
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

  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}


// prepare a web page to be send to a client (web browser)
String prepareHtmlPage()
{
  String value = (val) ? F("high") : F("low");
  String address = WiFi.localIP().toString();
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "\r\nGPIO is now "+
             value +
            "<br><br>Click <a href='http://"+
            address+
            "/gpio/1'>here</a> to switch LED GPIO on, or <a href='http://"+
            address+
            "/gpio/0'>here</a> to switch LED GPIO off" +
            "</html>" +
            "\r\n";
  return htmlPage;
}


void loop()
{
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    client.setTimeout(5000); // default is 1000
    int firstline = 0;
    
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);

        //evalua peticion de cliente para la primera linea
        if(firstline<1){
          // Match the request
          if (line.indexOf(F("/gpio/0")) != -1) {
            val = 0;
          } else if (line.indexOf(F("/gpio/1")) != -1) {
            val = 1;
          } else {
            Serial.println(F("invalid request"));
            val = digitalRead(LED);
          }
          // Set LED according to the request
          digitalWrite(LED, val);
          firstline=1;
        }
        
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareHtmlPage());
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
}
