#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>

#include "variables.h"

SoftwareSerial GeoSerial(3, 4);


#define pi 3.14159265358979323846

uint8_t ss = 10;   //LoRa SPI Chip Select Pin
uint8_t dio0 = 2;  //LoRa DIO0 Pin
uint8_t rst = 9;   //LoRa Reset Pin

#define buzzerPin 6


struct SlaveData_encrypt {
  uint8_t header0 = 0xFF;
  uint8_t header1 = 0xFF;
  uint8_t SlaveAssignment = 0;
  bool LocationValid = false;
  float slaveLat = 0.00;
  float slaveLon = 0.00;
  uint32_t count = 0;
  uint8_t footer = 0xFF;
};
typedef struct SlaveData_encrypt SlaveData_en;
SlaveData_en SlavePayload;
int SlaveDataSize = sizeof(struct SlaveData_encrypt);


struct MasterData_encrypt {
  uint8_t header0 = 0xCC;
  uint8_t header1 = 0xCD;
  char SlaveID[7] = "xxxxxx";
  char SlavePW[5] = "0000";
  uint8_t SlaveAssignment = 0;
  bool PairCommand = false;
  float GeofenceMidpoint_Lat = 0.0;
  float GeofenceMidpoint_Lon = 0.0;
  uint16_t GeofenceRad = 0;
  uint8_t footer = 0xDD;
};
typedef struct MasterData_encrypt MasterData_en;
MasterData_en MasterPayload;
int MasterDataSize = sizeof(struct MasterData_encrypt);



void setup() {
  // //init EEPROM for Slave Pairing
  // EEPROM.write(101,false);
  // EEPROM.write(102,false);
  // EEPROM.write(103,false);
  // EEPROM.write(104,false);
  // EEPROM.write(105,false);
  // EEPROM.write(106,false);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  GeoSerial.begin(9600);
  Serial.println("GeoFence MASTER DEVICE\n\n\nSystem Starting\n\n\n");
  for (int x = 1; x <= 6; x++) {
    SlaveStatus[x] = EEPROM.read(x + 100);
    Serial.println("STATUS Slave [" + String(x) + "]:" + String(SlaveStatus[x]));
  }
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

  if (!LoRa.begin(433E6)) {  //use (915E6) for LoRa Ra-02 915 MHz
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  Wire.begin();
  GeoFenceInterrupt();
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

      Serial.println("IIC Data Sent!");

      Serial.println("DONE READING " + String(NumberOfBytesToRead) + " DATA");

      Serial.print("DATA:");
      for (int a = 0; a < NumberOfBytesToRead; a++) {
        Serial.print(ch_GeoSerialDATA[a]);
      }
      Serial.println("\n\n");
      DecodeToFloat();
      SaveToEEPROM();
      LoRa_SendDataToSlave();
      delay(500);
      LoRa_SendDataToSlave();
      delay(500);
      LoRa_SendDataToSlave();
      delay(500);
      Serial.println("IIC SENDING DATA1...");
      Wire.beginTransmission(9);  // transmit to device #9
      Wire.write(0xAA);
      Wire.write(EEPROM.read(101));
      Wire.write(EEPROM.read(102));
      Wire.write(EEPROM.read(103));
      Wire.write(EEPROM.read(104));
      Wire.write(EEPROM.read(105));
      Wire.write(EEPROM.read(106));
      for (int count = 1; count < NumberOfBytesToRead - 14; count++) {
        Wire.write(ch_GeoSerialDATA[count]);
      }
      Wire.endTransmission();  // stop transmitting
    }
  }





  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    LoRa.readBytes((uint8_t *)&SlavePayload, packetSize);
    Serial.println("**********************************");
    Serial.print("Received packet:");
    Serial.write((uint8_t *)&SlavePayload, sizeof(SlavePayload));
    Serial.println("\n**********************************");
    Serial.println("###############################");
    Serial.println("Data Decryption Test\n\n");
    Serial.print("Count:");
    Serial.println(SlavePayload.count);
    Serial.print("HEADE0:");
    Serial.println(SlavePayload.header0, HEX);
    Serial.print("HEADER1:");
    Serial.println(SlavePayload.header1, HEX);
    Serial.print("Device Assignment:");
    Serial.println(SlavePayload.SlaveAssignment);
    Serial.print("Location Valid:");
    Serial.println(SlavePayload.LocationValid);
    Serial.print("Data Sample Latitude: ");
    Serial.println(SlavePayload.slaveLat, 6);
    Serial.print("Data Sample Longitude: ");
    Serial.println(SlavePayload.slaveLon, 6);
    Serial.print("FOOTER:");
    Serial.println(SlavePayload.footer, HEX);

    if (SlavePayload.SlaveAssignment == 99) {
      Serial.println("GEOSERIAL:SLAVEXXX");
      GeoSerial.print("SLAVEXXX");
      GeoSerial.print("SLAVEXXX");
      GeoSerial.print("SLAVEXXX");
    } else {
      GeoSerial.print("SLAVE");
      GeoSerial.print(String(SlavePayload.SlaveAssignment));
      // GeoSerial.print("SLAVE");
      // GeoSerial.print(String(SlavePayload.SlaveAssignment));
      // GeoSerial.print("SLAVE");
      // GeoSerial.print(String(SlavePayload.SlaveAssignment));
    }

    if (SlavePayload.header0 == 0xAA && SlavePayload.header1 == 0xAB && SlavePayload.footer == 0xBB) {
      Serial.print("Sample Link: https://maps.google.com/?q=");
      Serial.print(SlavePayload.slaveLat, 6);
      Serial.print(",");
      Serial.println(SlavePayload.slaveLon, 6);
    }
    Serial.println("###############################");
    Serial.println("\n\n\n\n\n\n");

    if (SlavePayload.LocationValid) {
      Serial.print("SLAVE DISTANCE FROM MIDPOINT:");
      Serial.print(calculateDistance(GeoFenceMidPoint_Latitude, GeoFenceMidPoint_Longitude, SlavePayload.slaveLat, SlavePayload.slaveLon));
      Serial.println(" meters");
      if (SlavePayload.SlaveAssignment == 99) {
        GeoSerial.print("SLAVEXXX");
      }
      GeoSerial.print(",");
      GeoSerial.print(SlavePayload.slaveLat, 6);
      GeoSerial.print(",");
      GeoSerial.print(SlavePayload.slaveLon, 6);

      DistanceFrom_GeoFenceMidpoint = calculateDistance(GeoFenceMidPoint_Latitude, GeoFenceMidPoint_Longitude, SlavePayload.slaveLat, SlavePayload.slaveLon);
      AlarmTypeMaster = true;
      if (DistanceFrom_GeoFenceMidpoint > GeoFenceRadius) {
        GeoAlarm = true;
        GeoAlarm_AlarmInterrupt = 2000;
      } else {
        GeoAlarm = false;
        GeoAlarm_AlarmInterrupt = 8000;
      }


      Wire.beginTransmission(9);  // transmit to device #9
      Wire.write(0xBB);
      Wire.write(SlavePayload.SlaveAssignment);

      Wire.write(GeoAlarm);

      Wire.endTransmission();  // stop transmitting
    } else {
      Serial.println("INVALID LOCATION cannot determine Distance from Midpoint");
      Wire.beginTransmission(9);  // transmit to device #9
      Wire.write(0xCC);
      Wire.write(SlavePayload.SlaveAssignment);
      Wire.write(GeoAlarm);
      Wire.endTransmission();  // stop transmitting
      AlarmTypeMaster = true;
      if (DistanceFrom_GeoFenceMidpoint > GeoFenceRadius) {
        GeoAlarm = true;
        GeoAlarm_AlarmInterrupt = 1000;
      } else {
        GeoAlarm = false;
        GeoAlarm_AlarmInterrupt = 4000
        ;
      }
    }
    alarm_last_millis = millis();
  }

  if(!SlaveStatus[0]&& !SlaveStatus[1] && !!SlaveStatus[2] && !SlaveStatus[3] && !SlaveStatus[4] && !SlaveStatus[5] && !SlaveStatus[6]){
    AlarmTypeMaster=false;
  }


  if (millis() - alarm_last_millis > 10000) {
    AlarmTypeMaster = false;
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
