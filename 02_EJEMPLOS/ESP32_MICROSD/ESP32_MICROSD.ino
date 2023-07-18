/*
* Código para el manejo de ficheros
*/
#include <SD.h>
#include "FS.h"
#include "SPI.h"

/***************** Constantes ******************/
#define LED 27
#define CS  5

/***************** Funciones ******************/
// Crear y escribir un archivo
void writeFile(fs::FS &fs, const char * path, String message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Leer un archivo
String readFile(fs::FS &fs, const char * path){
  String payload;
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    //Serial.println("Failed to open file for reading");
    return "Error en la lectura";
  }

  Serial.print("Read from file: ");
  char linea;
  int i = 1;
  while(file.available()){
    linea = file.read();
    payload += String(linea);
    i++;
    //Serial.print(linea);
  }
  file.close();
  return payload;
}

// Agregando líneas a un archivo
void appendFile(fs::FS &fs, const char * path, String message){
  Serial.printf("\nAppending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// Eliminar un archivo
void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

/***************** Configuración y prueba *******************/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  if(!SD.begin(CS)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  writeFile(SD, "/hello.txt", "Hello ");
  //readFile(SD, "/archivo.txt");
  //String resultados = readFile(SD, "/archivo.txt");
  //Serial.print(resultados);
  appendFile(SD, "/hello.txt", "World!\n");
  //readFile(SD, "/hello.txt");
  //deleteFile(SD, "/hello.txt");
  //readFile(SD, "/hello.txt");

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  digitalWrite(LED, HIGH);
}

/********************* Ciclo infinito **********************/
void loop() {
  // put your main code here, to run repeatedly:

}
