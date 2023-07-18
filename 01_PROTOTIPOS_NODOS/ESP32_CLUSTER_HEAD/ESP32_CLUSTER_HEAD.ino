/*
* Codigo para los nodos sensores
* Autor: Luis Castrillo
*/

/*
*************** Librerias ***************
*/
#include <OneWire.h>                      // DS18B20
#include <DallasTemperature.h>            // DS18B20
#include <Wire.h>                         // BME280 y TinyRTC
#include <Adafruit_Sensor.h>              // BME280
#include <Adafruit_BME280.h>              // BME280
#include <RTClib.h>                       // TinyRTC
#include <SD.h>                           // MicroSD
#include <FS.h>                           // MicroSD
#include <SPI.h>                          // MicroSD
#include <WiFi.h>                         // WiFi
#include <WebServer.h>                    // Servidor Web

/*
*************** Constantes y puertos GPIO ***************
*/
#define INTERVALO 5000                    // Periodo de medicion en milisegundos
#define DS18B20_GPIO 4                    // GPIO para conectar el DS18B20
#define ID_BME 0x76                       // Dirección del sensor BME280
#define PRESION_NIVEL_MAR (1013.25)       // Presion atmosferica al nivel del mar
#define CS 5                              // GPIO para controlar el modulo MicroSD
#define ARCHIVO ("/Datos.txt")            // Ruta del archivo donde se guardaran los datos de todos los nodos
#define ARCHIVO_NODO  ("/Nodo1.txt")      // Ruta del archivo donde se guardaran los datos del nodo
#define ID_NODO 1                         // Identificador del nodo sensor
#define LED_BUILTIN 2                     // GPIO del diodo LED para informar estado del sistema
#define WIFI_LED 15
#define RXD2 16                           // GPIO RX para recibir los datos de los nodos sensores
#define TXD2 17                           // GPIO TX para recibir los datos de los nodos sensores
#define SSID "<YOUR_SSID>"
#define PASSWORD "<YOUR_PASSWORD>"

/*
**************** Instancias ***************
*/
OneWire oneWire(DS18B20_GPIO);            // Objeto OneWire para DS18B20
DallasTemperature sensor_ds(&oneWire);    // Objeto DallasTemperature para DS18B20
Adafruit_BME280 sensor_bme;               // Objeto Adafruit_BME280 para BME280
RTC_DS1307 tiny_rtc;                      // Objeto RTC_DS1307 para TinyRTC
WebServer servidor(80);

/*
************** Variables globales *************
*/
unsigned long tiempo_anterior = 0;        // Variable de control de mediciones
unsigned long tiempo_actual = 0;          // Variable de control de mediciones
//float temperatura_ds;                     // Variable para guardar los valores del sensor DS18B20
//float temperatura_bme;                    // Variable para guardar la temperatura del BME280
//float humedad_bme;                        // Variable para guardar la humedad del BME280
//float presion_bme;                        // Variable para guardar la presion del BME280
//float altitud_bme;                        // Variable para guardar la altura del BME280
//String fecha_hora;                        // Variable para guardar la fecha y hora
boolean todo_ok = true;                   // Variable para saber que todo esta bien
/*
******** Funciones para leer datos del sensor DS18B20 *********
*/
// Obtener temperatura sensor DS18B20
float getTemperatureDS() {
  sensor_ds.requestTemperatures(); 
  float temperatura_celsius = sensor_ds.getTempCByIndex(0);
  return temperatura_celsius;
}

/*
******** Funciones para leer datos del sensor BME280 *********
*/
// Obtener temperatura en °C sensor BME280
float getTemperatureBME() {
  float temperatura_celsius = sensor_bme.readTemperature();
  return temperatura_celsius;
}

// Obtener humedad relativa sensor BME280
float getHumidityBME() {
  float humedad_relativa = sensor_bme.readHumidity();
  return humedad_relativa;
}

// Obtener presion atmosferica sensor BME280
float getPressureBME() {
  float presion_atmosferica = sensor_bme.readPressure()/100.0F;
  return presion_atmosferica;
}

// Obtener altitud sensor BME280
float getAltitudeBME() {
  float altitud = sensor_bme.readAltitude(PRESION_NIVEL_MAR);
  return altitud;
}

/*
************ Funcion para obtener la fecha y hora ****************
*/
// Obtener fecha y hora modulo TinyRTC
String getDatetime() {
  char respuesta[20];
  DateTime fecha_hora;
  fecha_hora = tiny_rtc.now();
  sprintf(respuesta, "%02d-%02d-%02dT%02d:%02d:%02d", fecha_hora.year(), 
  fecha_hora.month(), fecha_hora.day(), fecha_hora.hour(), fecha_hora.minute(), fecha_hora.second());
  return String(respuesta);
}

/*
*********** Funciones para controlar la lectura y escritura de datos *********
*/
// Funcion para crear un archivo
String createFile(fs::FS &fs, const char * path, const String mensaje) {
  // Abriendo conexion y creando archivo
  File file = fs.open(path, FILE_WRITE);
  // Comprobar creacion
  if (!file) {
    return "Error al crear el archivo " + String(path);
  }
  // Escribir mensaje en el archivo
  if (file.print(mensaje)) {
    // Cerrando conexion
    file.close();
    return "Mensaje escrito";
  } else {
    // Cerrando conexion
    file.close();
    return "Error al escribir el mensaje";
  }
}

// Funcion para leer un archivo
String readFile(fs::FS &fs, const char * path){
  String payload;
  // Abriendo conexion con el archivo
  File file = fs.open(path);
  // Comprobando si se pudo abrir el archivo
  if(file){
    while(file.available()){
      char linea = file.read();
      payload += String(linea);
    }
    // Cerrando conexion
    file.close();
    return payload;
  } else {
    file.close();
    return "Error leyendo archivo " + String(path);
  } 
}

// Funcion para agregar informacion a un archivo
String appendFile(fs::FS &fs, const char * path, const char * mensage){
  // Abriendo conexion con el archivo
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    return "Error al abrir el archivo " + String(path);
  }
  // Agregando mensaje
  if(file.print(mensage)){
    // Cerrando conexion
    file.close();
    return "Mensaje agregado";
  } else {// Cerrando conexion
    file.close();
    return "Error agregando mensaje";
  }
}

// Funcion para borrar un archivo
String deleteFile(fs::FS &fs, const char * path){
  // Borrando archivo
  if(fs.remove(path)){
    return "Archivo " + String(path) + " borrado";
  } else {
    return "Falla borrando el archivo " + String(path);
  }
}

// Funcion para verificar la existencia de un archivo
boolean checkFile(fs::FS &fs, const char * path){
  // Abriendo conexion con el archivo
  File file = fs.open(path);
  // No existe archivo
  if(!file){
    file.close();
    return false;
  }
  // Existe archivo
  if(file.available()){
    file.close();
    return true;
  } 
}

/*
***************** Funciones para el manejo del servidor *************************
*/
// Responder a la url raíz (root /)
void handleConnectionRoot() {
  // Nuestra respuesta en html
  String answer = "<!DOCTYPE html>\
  <html>\
  <body>\
  <h1>Hola desde ESP32 - Modo Punto de Acceso (AP)</h1>\
  </body>\
  </html>";
  servidor.send(200, "text/html", answer);
}

// Responder a la petición de datos
void handleDevice() {
  digitalWrite(WIFI_LED, HIGH);
  String payload = readFile(SD, ARCHIVO);
  servidor.send(200, "text/plain", payload);
  digitalWrite(WIFI_LED, LOW);
  deleteFile(SD, ARCHIVO);
  createFile(SD, ARCHIVO, "");
  Serial.println("Mensaje enviado");
}

// Archivo no encontrado
void handleNotFound(){
  servidor.send(404, "text/plain", "Not found");
}

/*
********* funcion Toggle LED ***********
*/
void toggleLed(int milisegundos) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milisegundos);
  digitalWrite(LED_BUILTIN, LOW);
  delay(milisegundos);
}

/*
**************** Funciones para la lectura, escritura y recepcion de datos ***********************
*/
// Funcion para armar la trama de datos
String measurements() {
  String fecha_hora = getDatetime();             // Obtener fecha y hora
  float temperatura_ds = getTemperatureDS();    // Leer temperatura sensor DS18B20
  float temperatura_bme = getTemperatureBME();  // Leer temperatura sensor BME280
  float humedad_bme = getHumidityBME();         // Leer humedad relativa sensor BME280
  float presion_bme = getPressureBME();         // Leer presion atmosferica sensor BME280
  float altitud_bme = getAltitudeBME();         // Leer altitud sensor BME280
  //Serial.println("************ Nueva medicion *************");
  //Serial.println("Fecha y hora: " + fecha_hora);
  //Serial.println("Temperatura DS18B20: " + String(temperatura_ds) + " °C");
  //Serial.println("Temperatura BME: " + String(temperatura_bme) + " °C");
  //Serial.println("Humedad relativa BME: " + String(humedad_bme) + " %");
  //Serial.println("Presion atmosferica BME: " + String(presion_bme) + " hPa");
  //Serial.println("Altitud BME: " + String(altitud_bme) + " m");
  String mensaje;
  mensaje.concat(String(ID_NODO));
  mensaje.concat(("," + fecha_hora));
  mensaje.concat(("," + String(temperatura_ds)));
  mensaje.concat(("," + String(temperatura_bme)));
  mensaje.concat(("," + String(humedad_bme)));
  mensaje.concat(("," + String(presion_bme)));
  mensaje.concat(("," + String(altitud_bme) + "\n"));
  return mensaje;
}

// Funcion para tomar los primeros datos
void getInitData() {
  digitalWrite(LED_BUILTIN, LOW);
  String mensaje = measurements();
  // Crear un nuevo archivo si este aun no esta creado
  if(!checkFile(SD, ARCHIVO)) {
    String status;
    status = createFile(SD, ARCHIVO, mensaje);
    Serial.println(status);
  } else {
    appendFile(SD, ARCHIVO, mensaje.c_str());
  }

  if(!checkFile(SD, ARCHIVO_NODO)) {
    String status;
    status = createFile(SD, ARCHIVO_NODO, mensaje);
    Serial.println(status);
  } else {
    appendFile(SD, ARCHIVO_NODO, mensaje.c_str());
  }
  Serial.print(mensaje);
  digitalWrite(LED_BUILTIN, HIGH);
}

// Funcion para tomar datos periodicamente
void getData() {
  digitalWrite(LED_BUILTIN, LOW);
  String mensaje = measurements();
  appendFile(SD, ARCHIVO, mensaje.c_str());
  appendFile(SD, ARCHIVO_NODO, mensaje.c_str());
  Serial.print(mensaje);
  digitalWrite(LED_BUILTIN, HIGH);
}

void incomingData() {
  String mensaje;
  while(Serial2.available()) {
    char letra = (char)Serial2.read();
    mensaje.concat(letra);
  }
  appendFile(SD, ARCHIVO, mensaje.c_str());
  Serial.print(mensaje);
}
/*
************ Funciones de inicializacion **************
*/
// Funcion para inicializar sensor BME
void initBME() {
  if (!sensor_bme.begin(ID_BME)) {
    Serial.println("El sensor BME280 no esta conectado o esta averiado. Verifica!");
    todo_ok = false;
    while(1) {
      toggleLed(250);
    }
    //return;
  }
}

// Funcion para inicializar modulo TinyRTC
void initRTC() {
  if (!tiny_rtc.begin()) {
    Serial.println("El modulo TinyRTC no esta conectado o esta averiado. Verifica!");
    todo_ok = false;
    while(1) {
      toggleLed(500);
    }
    //return;
  }
}

// Funcion para inicializar modulo microSD
void initSD() {
  if(!SD.begin(CS)){
    Serial.println("El modulo o la tarjeta MicroSD no estan conectado. Verifica!");
    todo_ok = false;
    while(1) {
      toggleLed(1000);
    }
    //return;
  }
}

// Funcion para inicializar servidor
void initWebServer() {
  WiFi.softAP(SSID, PASSWORD);
  Serial.println("SSID: " + String(SSID));
  Serial.println("IP: " + String(WiFi.softAPIP()));
 
  servidor.on("/", handleConnectionRoot);
  servidor.on("/datos", handleDevice);
  servidor.onNotFound(handleNotFound);
  servidor.begin();

  Serial.println("Servidor web iniciado");
}

/*
************************** Rutina de configuración ***************************
*/
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Inicializando el GPIO del LED ESTADO
  pinMode(WIFI_LED, OUTPUT);        // Inicializando el GPIO del LED WIFI
  Serial.begin(115200);             // Inicializando puerto serial a 115200 Baudios
  Wire.begin();                     // Inicializando puerto Wire
  // Inicializando el sensor DS18B20
  sensor_ds.begin();                
  // Comprobando conexion con sensor BME280
  initBME();
  // Comprobando conexion con el TinyRTC
  initRTC();
  // Comprobando conexion con modulo MicroSD
  initSD();
  // Tomar datos iniciales
  if(todo_ok) {
    digitalWrite(LED_BUILTIN, HIGH);
    getInitData();
    tiempo_anterior = millis();
  }
  // Inicializando puerto serial2 a 115200 Baudios
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  // Inicializando servidor web
  initWebServer();
}

/*
*************************** Rutina infinita **************************
*/
void loop() {
  servidor.handleClient();
  if(todo_ok) {
    incomingData();
    tiempo_actual = millis();
    if(tiempo_actual >= (tiempo_anterior + INTERVALO)) {
      tiempo_anterior = tiempo_actual;
      getData();
    }
  }
}
