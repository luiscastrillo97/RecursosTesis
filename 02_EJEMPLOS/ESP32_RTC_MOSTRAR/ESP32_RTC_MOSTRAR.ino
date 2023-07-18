/*
* Código para mostrar la hora y fecha del módulo RTC
*/

// Librería para la comunicación I2C y la RTClib
#include <Wire.h>
#include <RTClib.h>
 
// Declaramos un RTC DS3231
RTC_DS1307 rtc;
DateTime now;
char str[20];
unsigned long previousMillis = 0;        // will store last time LED was updated
const long intervalo = 1000;
 
void setup () {
  Wire.begin();
  Serial.begin(115200);
  while(!Serial) {
    Serial.print("."); // Espera hasta que el puerto serial se conecte
  }
 
  delay(100);
 
  // Comprobamos si tenemos el RTC conectado
  if (! rtc.begin()) {
    Serial.println("No hay un módulo RTC");
    while (1);
  }
}
 
void loop () {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalo) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    now = rtc.now();
    sprintf(str, "%02d-%02d-%02dT%02d:%02d:%02d", now.year(), 
    now.month(), now.day(), now.hour(), now.minute(), now.second());
    Serial.println(str);
  }
}