/*
* Conexion con el sensor BME280
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

#define ID_BME 0x76
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Tiempo de medicion
#define INTERVALO 5000
unsigned long tiempo_anterior = 0;
unsigned long tiempo_actual = 0;

void setup() {
  Serial.begin(115200);

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  bool status_BME;
  status_BME = bme.begin(ID_BME);  
  if (!status_BME) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    return;
  }
}


void loop() { 
  tiempo_actual = millis();
  if(tiempo_actual >= (tiempo_anterior + INTERVALO)) {
    tiempo_anterior = tiempo_actual;
    getDataBME();
  }
}

void getDataBME() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}