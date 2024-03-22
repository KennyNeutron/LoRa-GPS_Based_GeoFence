#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial GeoSerial(3, 4);

uint32_t ihap = 0;
uint8_t GeoSerialDATA = 0xFF;

uint16_t NumberOfBytesToRead = 0;

char ch_GeoSerialDATA[100] = "";

float GeoFenceMidPoint_Latitude = 0.00;
float GeoFenceMidPoint_Longitude = 0.00;
uint16_t GeoFenceRadius = 0;

String str_GeoFenceMidPoint_Latitude = "";
String str_GeoFenceMidPoint_Longitude = "";
String str_GeoFenceRadius = "";

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



}


void SaveToEEPROM() {
  EEPROM.write(0, NumberOfBytesToRead);

  for (int x = 0; x < NumberOfBytesToRead; x++) {
    EEPROM.write(x + 1, ch_GeoSerialDATA[x]);
  }
  Serial.println("DONE SAVING " + String(NumberOfBytesToRead) + "bytes of data to EEPROM");
}
