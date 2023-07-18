/*
* Codigo para el nodo sumidero
* Autor: Luis Castrillo
*/

/*
*************** Librerias ***************
*/  
#include <WiFi.h>             // WiFi
#include <HTTPClient.h>       // HTTP
#include "FS.h"               // MicroSD
#include "SD.h"               // MicroSD
#include "SPI.h"              // MicroSD

/*
*************** Constantes y puertos GPIO ***************
*/
#define CS 5                              // GPIO para controlar el modulo MicroSD
#define PATH ("/datosSumidero.txt")       // Ruta del archivo donde se guardaran los datos
#define SSID "<YOUR_SSID>"                        // SSID de la red
#define PASSWORD "<YOUR_PASSWORD>"             // Contraseña de la red
#define URL "http://192.168.4.1/datos"    // URL para acceder a los datos
#define LED_BUILTIN 27                    // LED indicador

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
    return "Mensaje escrito";
  } else {
    return "Error al escribir el mensaje";
  }
  // Cerrando conexion
  file.close();
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
************* Funcion para realizar la peticion de los datos *************
*/
String getRequest(const char* url) {
  HTTPClient http;        //Instanciar objeto HTTPClient
  http.begin(url);        //Iniciamos la conexión http con el nombre del servidor

  //Enviamos petición HTTP
  int httpResponseCode = http.GET();
  String payload = "...";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  //Terminammos conexión HTTP
  http.end();
  return payload;
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
************ Funciones de inicializacion **************
*/
// Funcion para inicializar modulo microSD
void initSD() {
  if(!SD.begin(CS)){
    Serial.println("El modulo o la tarjeta MicroSD no estan conectado. Verifica!");
    while(1) {
      toggleLed(1000);
    }
    //return;
  }
}

// Funcion para inicializar Wifi y cliente HTTP
void initWifi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Conectando");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("/nWiFi Conectado!");
}

// Creacion del archivo de almacenamiento
void createStorage() {
  if(!checkFile(SD, PATH)) {
    String columnas = "Id,FechaHora,TemperaturaDS,TemperaturaBME,HumedadBME,PresionBME,ALtitudBME\n";
    createFile(SD, PATH, columnas);
  }
}

/*
************************** Rutina de configuración ***************************
*/
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //Inicializando el puerto serial
  Serial.begin(115200);
  //Inicializando SD
  initSD();
  // Creacion del archivo para guardar los datos
  createStorage();
  //Todo OK
  digitalWrite(LED_BUILTIN, HIGH);
  return;
}

void loop() {
  // Realizar peticion
  //Inicializando Wifi
  initWifi();
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    String respuesta = getRequest(URL);
    String status = appendFile(SD, PATH, respuesta.c_str());
    Serial.println(status);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  while(WiFi.status() == WL_CONNECTED){
    // Continua en este ciclo hasta que se desconecte
  };
}  


