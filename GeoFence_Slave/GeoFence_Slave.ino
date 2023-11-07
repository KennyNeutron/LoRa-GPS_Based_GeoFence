/*
  LoRa RA-02 SX1278 Sample Code --> SLAVE DEVICE Sender
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
  uint8_t id = 0x01;
  uint8_t sampeUI8 = 0x55;
  float sampleF = 33.123456;
  String sampleS = "Hello World";
};

typedef struct data_encrypt Data_en;

Data_en payload;

int datasize = sizeof(struct data_encrypt);

int counter = 0;

void setup() {
  Serial.begin(115200);

  while (!Serial)
    ;

  Serial.println("GeoFence MASTER DEVICE\n\n\n");
  
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  Serial.println("###############################");
  Serial.println("Data Encryption Test\n\n");
  Serial.print("Data ID: ");
  Serial.println(payload.id);
  Serial.print("Data Sample UINT8_T: ");
  Serial.println(payload.sampeUI8);
  Serial.print("Data Sample Float: ");
  Serial.println(payload.sampleF);
  Serial.print("Data Sample String:");
  Serial.println(payload.sampleS);
  Serial.println("###############################");
}

void loop() {
  Serial.print("\n\n\nSending packet: ");
  Serial.write((uint8_t*)&payload, sizeof(payload));


  // send packet
  LoRa.beginPacket();
  LoRa.write((uint8_t*)&payload, sizeof(payload));
  LoRa.endPacket();

  delay(2000);
}
