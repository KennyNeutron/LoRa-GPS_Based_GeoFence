/*
  LoRa RA-02 SX1278 Sample Code --> Receiver
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
  uint8_t id = 0x00;
  uint8_t sampeUI8 = 0x00;
  //float sampleF = 1.23;
  //String sampleS = "Hello World";
};

typedef struct data_encrypt Data_en;

Data_en payload;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("LoRa Receiver");

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
    Serial.println("Received packet:");
    Serial.write((uint8_t *)&payload, sizeof(payload));
    Serial.print("\n\n\n");
    Serial.println("###############################");
    Serial.println("Data Decryption Test\n\n");
    Serial.print("Data ID: ");
    Serial.println(payload.id, HEX);
    Serial.print("Data Sample UINT8_T: ");
    Serial.println(payload.sampeUI8, HEX);
    //Serial.print("Data Sample Float: ");
    //Serial.println(payload.sampleF);
    //Serial.print("Data Sample String:");
    //Serial.println(payload.sampleS);
    Serial.println("###############################");
  }
}