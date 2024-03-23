#include <SPI.h>
#include <LoRa.h>

#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "variables.h"

SoftwareSerial GeoSerial(3, 4);

#define pi 3.14159265358979323846

uint8_t ss = 10;    //LoRa SPI Chip Select Pin
uint8_t dio0 = 2;   //LoRa DIO0 Pin
uint8_t rst = 9;    //LoRa Reset Pin


struct data_encrypt {
  uint8_t header0 = 0xFF;
  uint8_t header1 = 0xFF;
  uint32_t id = 0xFFFFFFFF;
  bool LocationValid = false;
  float slaveLat = 0.00;
  float slaveLon = 0.00;
  uint32_t count = 0;
  uint8_t footer = 0xFF;
};

typedef struct data_encrypt Data_en;

Data_en payload;



void setup() {
  Serial.begin(9600);
  GeoSerial.begin(9600);
  Serial.println("System Starting");

  for (int i = 0; i <= 10; i++) {
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
  }
  digitalWrite(13, 0);

  Serial.println("Stored EEPROM data= " + String(EEPROM.read(0)) + "bytes");
  Serial.println("RETRIEVING DATA...");
  for (int i = 0; i < EEPROM.read(0); i++) {
    ch_GeoSerialDATA[i] = EEPROM.read(i + 1);
  }
  Serial.println("=======================");
  DecodeToFloat();
  Serial.println("=======================");


  Serial.println("LoRa Receiver");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) { //use (915E6) for LoRa Ra-02 915 MHz
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}


void loop() {

  if (GeoSerial.available() > 0) {
    Serial.println("RECIEVED");
    GeoSerialDATA = GeoSerial.read();
    Serial.println(GeoSerialDATA, DEC);

    if (GeoSerialDATA == 0xAA) {
      Serial.println("HEAD");
      GeoSerialDATA = GeoSerial.read();
      NumberOfBytesToRead = GeoSerialDATA;
      GeoSerialDATA = GeoSerial.read();
      if (GeoSerialDATA == 0xAB) {
        Serial.println("FOOT");
        Serial.println("Bytes to READ:" + String(NumberOfBytesToRead));
      }

      for (int i = 0; i < NumberOfBytesToRead; i++) {
        while (GeoSerial.available() == 0) {
          delayMicroseconds(10);
        }
        ch_GeoSerialDATA[i] = GeoSerial.read();
      }

      Serial.println("DONE READING " + String(NumberOfBytesToRead) + " DATA");

      Serial.print("DATA:");
      for (int a = 0; a < NumberOfBytesToRead; a++) {
        Serial.print(ch_GeoSerialDATA[a]);
      }
      Serial.println("\n\n");
      DecodeToFloat();
      SaveToEEPROM();
    }
  }



  int packetSize = LoRa.parsePacket();
  if (packetSize)  // Only read if there is some data to read..
  {
    LoRa.readBytes((uint8_t *)&payload, packetSize);
    Serial.println("**********************************");
    Serial.print("Received packet:");
    Serial.write((uint8_t *)&payload, sizeof(payload));
    Serial.println("\n**********************************");
    Serial.println("###############################");
    Serial.println("Data Decryption Test\n\n");
    Serial.print("Data ID: ");
    Serial.println(payload.id, HEX);
    Serial.print("Count:");
    Serial.println(payload.count);
    Serial.print("HEADE0:");
    Serial.println(payload.header0, HEX);
    Serial.print("HEADER1:");
    Serial.println(payload.header1, HEX);
    Serial.print("Location Valid:");
    Serial.println(payload.LocationValid);
    Serial.print("Data Sample Latitude: ");
    Serial.println(payload.slaveLat, 6);
    Serial.print("Data Sample Longitude: ");
    Serial.println(payload.slaveLon, 6);
    Serial.print("FOOTER:");
    Serial.println(payload.footer, HEX);
    if (payload.header0 == 0xAA && payload.header1 == 0xAB && payload.footer == 0xBB) {
      Serial.print("Sample Link: https://maps.google.com/?q=");
      Serial.print(payload.slaveLat, 6);
      Serial.print(",");
      Serial.println(payload.slaveLon, 6);
    }
    Serial.println("###############################");
    Serial.println("\n\n\n\n\n\n");

    if (payload.LocationValid) {
      Serial.print("SLAVE DISTANCE FROM MIDPOINT:");
      Serial.print(calculateDistance(GeoFenceMidPoint_Latitude, GeoFenceMidPoint_Longitude, payload.slaveLat, payload.slaveLon));
      Serial.println(" meters");
    } else {
      Serial.println("INVALID LOCATION cannot determine Distance from Midpoint");
    }


  }



}

float calculateDistance(float lat1, float long1, float lat2, float long2) {
  float dist;
  dist = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(long1 - long2);
  dist = acos(dist);
  dist = (6371 * pi * dist) / 180;
  return dist * 1000;
}



void SaveToEEPROM() {
  EEPROM.write(0, NumberOfBytesToRead);

  for (int x = 0; x < NumberOfBytesToRead; x++) {
    EEPROM.write(x + 1, ch_GeoSerialDATA[x]);
  }
  Serial.println("DONE SAVING " + String(NumberOfBytesToRead) + "bytes of data to EEPROM");
}
