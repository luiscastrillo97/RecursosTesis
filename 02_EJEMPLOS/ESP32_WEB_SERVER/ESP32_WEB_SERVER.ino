/*
* Código para configurar la ESP32 como Access Point y Web Server
*/

// Librerías
#include <WiFi.h>
#include <WebServer.h>

// LED Indicador
#define LED 27
 
// Creamos nuestra propia red -> SSID & Password
const char* ssid = "<YOUR_SSID>";  
const char* password = "<YOUR_PASSWORD>";
 
WebServer server(80);  // puerto por defecto 80

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
 
  // Creamos el punto de acceso
  WiFi.softAP(ssid, password); // Tiene mas parametros opcionales
  IPAddress ip = WiFi.softAPIP();
  Serial.println(ip);
 
 
  Serial.print("Nombre de mi red esp32: ");
  Serial.println(ssid);
 
  server.on("/", handleConnectionRoot);
  server.on("/datos", handleDevice);
  server.onNotFound(handleNotFound);
 
  server.begin();
  Serial.println("Servidor HTTP iniciado");
  delay(150);
  digitalWrite(LED, HIGH);
}
 
void loop() {
  server.handleClient();
}
  
// Responder a la url raíz (root /)
void handleConnectionRoot() {
  // Nuestra respuesta en html
  /*String answer = "<!DOCTYPE html>\
  <html>\
  <body>\
  <h1>Hola desde ESP32 - Modo Punto de Acceso (AP)</h1>\
  </body>\
  </html>";*/
  String answer = "Hola desde ESP32 - Modo Punto de Acceso (AP)";
  server.send(200, "text/plain", answer);
  Serial.println("Mensaje enviado");
  Serial.println(answer);
}

void handleDevice() {
  String payload = "1,1,0:0:0 10/10/2022,23.4,75.23,3.8\r\n\
2,1,1:0:0 10/10/2022,23.7,77.89,3.9\r\n\
3,1,2:0:0 10/10/2022,24.4,74.56,4.1\r\n\
4,1,3:0:0 10/10/2022,26.5,75.67,4.8\r\n\
5,1,4:0:0 10/10/2022,25.4,73.54,5.8\r\n\
6,1,5:0:0 10/10/2022,23.2,74.87,3.6\r\n\
7,1,6:0:0 10/10/2022,24.6,76.56,3.8\r\n\
8,1,7:0:0 10/10/2022,25.4,75.67,3.5\r\n\
9,1,8:0:0 10/10/2022,23.5,72.45,3.9\r\n\
10,1,9:0:0 10/10/2022,27.3,77.32,4.1\r\n\
11,1,10:0:0 10/10/2022,28.2,75.78,4.5\r\n\
12,1,11:0:0 10/10/2022,27.4,76.54,4.0\r\n\
13,1,12:0:0 10/10/2022,25.6,75.87,3.8\r\n\
14,1,13:0:0 10/10/2022,25.5,78.23,3.7\r\n\
15,1,14:0:0 10/10/2022,28.4,76.65,3.9\r\n\
16,1,15:0:0 10/10/2022,25.1,75.87,4.0\r\n\
17,1,16:0:0 10/10/2022,27.7,72.56,4.2\r\n\
18,1,17:0:0 10/10/2022,28.9,75.78,3.8\r\n\
19,1,18:0:0 10/10/2022,24.8,74.75,3.6\r\n\
20,1,19:0:0 10/10/2022,23.8,75.10,4.2\r\n\
21,1,20:0:0 10/10/2022,22.4,78.65,4.0\r\n\
22,1,21:0:0 10/10/2022,22.6,75.23,4.3\r\n\
23,1,22:0:0 10/10/2022,21.3,73.34,3.9\r\n\
24,1,23:0:0 10/10/2022,22.1,75.90,4.4\r\n";
  server.send(200, "text/plain", payload);
}

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}
