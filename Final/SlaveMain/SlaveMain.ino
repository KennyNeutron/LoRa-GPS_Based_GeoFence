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


//======================================= OLED ==================================================
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST); // Fast I2C / TWI
//===============================================================================================


uint8_t ss = 10;   //LoRa SPI Chip Select Pin
uint8_t dio0 = 4;  //LoRa DIO0 Pin
uint8_t rst = 9;   //LoRa Reset Pin

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
static const int RXPin = 2, TXPin = 3;

SoftwareSerial SerialGPS(RXPin, TXPin);

bool valid_location = false;
String gps_lat = " ";
String gps_lon = " ";

uint32_t iter = 0;


struct data_encrypt {
  uint8_t header0 = 0xAA;
  uint8_t header1 = 0xAB;
  uint32_t id = 0xFFFFFFFF;
  bool LocationValid = false;
  float slaveLat = gps.location.lat();
  float slaveLon = gps.location.lng();
  uint32_t count = iter;
  uint8_t footer = 0xBB;
};

typedef struct data_encrypt Data_en;

Data_en payload;
int datasize = sizeof(struct data_encrypt);
int counter = 0;

float GPS_Lat = 0.0;
float GPS_Lon = 0.0;

uint32_t DeviceID = 0xABCDEF01;

#define BuzzerPin 5

uint32_t last_millis = 0;
bool BuzzerToggle = false;
uint16_t BuzzerInterval = 100;

uint32_t LoRa_last_millis = 0;
uint16_t LoRa_SendInterval = 2000;

void setup() {
  pinMode(BuzzerPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("GeoFence SLAVE DEVICE\n\n\n");

  SerialGPS.begin(9600);
  Serial.println(TinyGPSPlus::libraryVersion());





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
  Serial.println("###############################");




  //============================ OLED =============================
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
  //===============================================================

  LoRa_last_millis = millis();
}

void loop() {
  if (!BuzzerToggle) {
    analogWrite(BuzzerPin, 100);
    delay(100);
    analogWrite(BuzzerPin, 255);
    BuzzerToggle = true;
    last_millis = millis();
  } else {
    if ((millis() - last_millis) > BuzzerInterval) {
      BuzzerToggle = false;
    }
  }


  Serial.println("executing gps_loop()");
  gps_loop();

  //=============== SEND PACKET =====================
  // send packet
  if (valid_location && (millis() - LoRa_last_millis) > LoRa_SendInterval) {
    Serial.println("Sending LoRa Packet");
    payload.count = iter;
    payload.id = DeviceID;

    LoRa.beginPacket();
    LoRa.write((uint8_t*)&payload, sizeof(payload));
    LoRa.endPacket();

    LoRa_last_millis = millis();
    iter++;
  }
  //=================================================


  /*
    if (valid_location) {
    digitalWrite(LEDloc, 1);
    Serial.println("Valid Loc");
    } else {
    digitalWrite(LEDloc, 0);
    }*/

  char ch_GPS_Lat[20] = {' '};
  int int_GPS_Lat = (int) GPS_Lat;
  float f_GPS_lat = (abs(GPS_Lat) - abs(int_GPS_Lat)) * 100000;
  int fra_GPS_lat = (int)f_GPS_lat;
  sprintf(ch_GPS_Lat, "%d.%d", int_GPS_Lat, fra_GPS_lat);

  char ch_GPS_Lon[20] = {' '};
  int int_GPS_Lon = (int) GPS_Lon;
  float f_GPS_Lon = (abs(GPS_Lon) - abs(int_GPS_Lon)) * 100000;
  int fra_GPS_Lon = (int)f_GPS_Lon;
  sprintf(ch_GPS_Lon, "%d.%d", int_GPS_Lon, fra_GPS_Lon);

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_helvR08);
    u8g.drawStr(0, 10, "GPS Loc. Valid:");
    if (valid_location) {
      u8g.drawStr(75, 10, "TRUE");
    } else {
      u8g.drawStr(75, 10, "FALSE");
    }
    u8g.drawStr( 0, 23, "GPS Lat:");
    u8g.drawStr( 50, 23, ch_GPS_Lat);
    u8g.drawStr( 0, 36, "GPS Lon:");
    u8g.drawStr( 50, 36, ch_GPS_Lon);
  } while ( u8g.nextPage() );

}
