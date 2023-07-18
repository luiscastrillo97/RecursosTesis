/*
* Código para la configuración del módulo RTC
* Hora de Internet
*/

// Librería para la comunicación I2C y la RTClib
#include <Wire.h>
#include <RTClib.h>
#include <ESP32Time.h>
#include <WiFi.h>
 
// Declaramos un RTC DS3231
RTC_DS1307 rtc;
DateTime now;
char str[20];
unsigned long tiempo_anterior = 0;
unsigned long tiempo_actual = 0;
const long intervalo = 1000;

// Variable para adquirir la fecha y hora por internet
ESP32Time tiempo_online;
const char* servidor_ntp = "pool.ntp.org";
const long zona_horaria = -5*3600;
const int day_light_offset_sec = 0;

// Conexion WiFi
const char* ssid = "<YOUR_SSID>";  
const char* password = "<YOUR_PASSWORD>";
 
void setup () {
  Wire.begin();
  delay(100);

  Serial.begin(115200);
  while(!Serial) {
    Serial.print("."); // Espera hasta que el puerto serial se conecte
  }
  delay(100);

  //Conectando el WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectando");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");  Serial.println("WiFi Conectado!");
  delay(100);

  configTime(zona_horaria, day_light_offset_sec, servidor_ntp);
  struct tm info_tiempo;
  if(getLocalTime(&info_tiempo)) {
    tiempo_online.setTimeStruct(info_tiempo);
  }
  delay(100);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
 
  // Comprobamos si tenemos el RTC conectado
  if (! rtc.begin()) {
    Serial.println("No hay un módulo RTC");
    while (1);
  }
   
  // Ponemos en hora, solo la primera vez, luego comentar y volver a cargar.
  // Ponemos en hora con los valores de la fecha y la hora en que el sketch ha sido compilado.
  rtc.adjust(DateTime(tiempo_online.getYear(), tiempo_online.getMonth() + 1, tiempo_online.getDay(), tiempo_online.getHour(true), tiempo_online.getMinute(), tiempo_online.getSecond()));
}
 
void loop () {
  tiempo_actual = millis();
  if (tiempo_actual >= (tiempo_anterior + intervalo)) {
    // save the last time you blinked the LED
    tiempo_anterior = tiempo_actual;
    now = rtc.now();
    sprintf(str, "%02d-%02d-%02dT%02d:%02d:%02d", now.year(), 
    now.month(), now.day(), now.hour(), now.minute(), now.second());
    Serial.println(str);
  }
}