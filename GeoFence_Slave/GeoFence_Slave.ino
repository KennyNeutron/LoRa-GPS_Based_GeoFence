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
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#define LEDloc 5

uint8_t ss = 10;   //LoRa SPI Chip Select Pin
uint8_t dio0 = 2;  //LoRa DIO0 Pin
uint8_t rst = 9;   //LoRa Reset Pin

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
static const int RXPin = 3, TXPin = 4;

SoftwareSerial SerialGPS(RXPin, TXPin);

bool valid_location = false;
String gps_lat = " ";
String gps_lon = " ";

uint32_t iter = 0;


struct SlaveData_encrypt {
  uint8_t header0 = 0xFF;
  uint8_t header1 = 0xFF;
  uint32_t id = 0xFFFFFFFF;
  bool LocationValid = false;
  float slaveLat = 0.00;
  float slaveLon = 0.00;
  uint32_t count = 0;
  uint8_t footer = 0xFF;
};

typedef struct SlaveData_encrypt SlaveData_en;

SlaveData_en SlavePayload;

int datasize = sizeof(struct SlaveData_encrypt);

int counter = 0;

void setup() {
  for (int e = 0; e < 255; e++) {
    EEPROM.write(e, 0);
  }

  pinMode(LEDloc, OUTPUT);
  Serial.begin(115200);

  SerialGPS.begin(9600);
  Serial.println(TinyGPSPlus::libraryVersion());



  Serial.println("GeoFence MASTER DEVICE\n\n\n");

  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  digitalWrite(LEDloc, 1);
  Serial.println("###############################");
  Serial.println("Data Encryption Test\n\n");
  Serial.print("Data ID: ");
  Serial.println(SlavePayload.id);
  Serial.print("HEADE0:");
  Serial.println(SlavePayload.header0, HEX);
  Serial.print("HEADER1:");
  Serial.println(SlavePayload.header1, HEX);
  Serial.print("Location Valid:");
  Serial.println(SlavePayload.LocationValid);
  Serial.print("Data Sample Latitude: ");
  Serial.println(SlavePayload.slaveLat, 6);
  Serial.print("Data Sample Longitude: ");
  Serial.println(SlavePayload.slaveLon, 6);
  Serial.print("FOOTER:");
  Serial.println(SlavePayload.footer, HEX);
  Serial.println("###############################");
  digitalWrite(LEDloc, 0);
}

void loop() {
  gps_loop();

  //Serial.print("\n\n\nSending packet: ");
  //Serial.write((uint8_t*)&payload, sizeof(payload));

  if (valid_location) {
    // send packet
    LoRa.beginPacket();
    LoRa.write((uint8_t*)&SlavePayload, sizeof(SlavePayload));
    LoRa.endPacket();

    delay(2000);


    iter++;
  }


  SlavePayload.count = iter;
  /*
    if (valid_location) {
    digitalWrite(LEDloc, 1);
    Serial.println("Valid Loc");
    } else {
    digitalWrite(LEDloc, 0);
    }*/
}
