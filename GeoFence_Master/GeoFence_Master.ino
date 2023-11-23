/*
  LoRa RA-02 SX1278 Sample Code --> MASTER DEVICE Receiver
  Date: 11-07-2023

  CONNECTIONS (Arduino Uno / Arduino Nano)

  LoRa ========== Arduino
  GND   --------  GND
  3.3V  --------  3.3V
  NSS   --------  10
  MOSI  --------  11
  MISO  --------  12
  SCK   --------  13
  RST   --------  9
  DIO0  --------  2

  NOTE: For STABILITY Connect All GND pin on the LoRa to the Arduino GND
*/

#include <SPI.h>
#include <LoRa.h>


uint8_t ss = 10;   //LoRa SPI Chip Select Pin
uint8_t dio0 = 2;  //LoRa DIO0 Pin
uint8_t rst = 9;   //LoRa Reset Pin

struct data_encrypt {
  uint8_t header0 = 0xFF;
  uint8_t header1 = 0xFF;
  uint8_t id = 0xFF;
  bool LocationValid = false;
  float slaveLat = 0.00;
  float slaveLon = 0.00;
  uint32_t count = 0;
  uint8_t footer = 0xFF;
};

typedef struct data_encrypt Data_en;

Data_en payload;

void setup() {
  Serial.begin(115200);

  Serial.println("GeoFence MASTER DEVICE");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {  //use (915E6) for LoRa Ra-02 915 MHz
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

void loop() {
  // try to parse packet
  /*int packetSize = LoRa.parsePacket();
    if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    }*/

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
    Serial.println(payload.id);
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
  }
}
