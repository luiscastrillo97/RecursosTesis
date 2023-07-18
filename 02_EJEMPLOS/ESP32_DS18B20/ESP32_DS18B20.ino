/*
* Lectura de temperatura con el sensor DS18B20
*/

// Librerias a utilizas
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO donde se conecta el DS18B20
#define DS18B20_GPIO 4

// Intervalo de medicion
#define INTERVALO 5000
unsigned long tiempo_anterior = 0;
unsigned long tiempo_actual = 0;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(DS18B20_GPIO);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensor(&oneWire);

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);

  // Start the DS18B20 sensor
  sensor.begin();
}

void loop() {
  tiempo_actual = millis();
  if(tiempo_actual >= (tiempo_anterior + INTERVALO)) {
    tiempo_anterior = tiempo_actual;
    getDataDS();
  }
}

void getDataDS() {
  sensor.requestTemperatures(); 
  float temperaturaC = sensor.getTempCByIndex(0);
  float temperaturaF = sensor.getTempFByIndex(0);
  Serial.println("Nueva medicion");
  Serial.println("Temperatura ");
  Serial.print(temperaturaC);
  Serial.println("ºC");
  Serial.println("Temperatura ");
  Serial.print(temperaturaF);
  Serial.println("ºF");
}