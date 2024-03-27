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
#include <EEPROM.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#include "variables.h"

//======================================= OLED ==================================================
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);  // Fast I2C / TWI
//===============================================================================================


uint8_t ss = 10;   //LoRa SPI Chip Select Pin
uint8_t dio0 = 4;  //LoRa DIO0 Pin
uint8_t rst = 9;   //LoRa Reset Pin

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
static const int RXPin = 2, TXPin = 3;

SoftwareSerial SerialGPS(RXPin, TXPin);




struct SlaveData_encrypt {
  uint8_t header0 = 0xAA;
  uint8_t header1 = 0xAB;
  uint8_t SlaveAssignment = 0;
  bool LocationValid = false;
  float slaveLat = gps.location.lat();
  float slaveLon = gps.location.lng();
  uint32_t count = iter;
  uint8_t footer = 0xBB;
};
typedef struct SlaveData_encrypt SlaveData_en;
SlaveData_en SlavePayload;



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

String thisSlave_ID = "aabbcc";
String thisSlave_PinCode = "8998";


void setup() {
  /*
  for (int e = 0; e < 255; e++) {
    EEPROM.write(e, 0);
  }
  */

  pinMode(BuzzerPin, OUTPUT);
  analogWrite(BuzzerPin, 255);

  Serial.begin(9600);
  Serial.println("GeoFence SLAVE DEVICE\n\n\nSystem Starting\n\n\n");

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
  Serial.print("\nHEADER0:");
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


  //============================ OLED =============================
  if (u8g.getMode() == U8G_MODE_R3G3B2) {
    u8g.setColorIndex(255);  // white
  } else if (u8g.getMode() == U8G_MODE_GRAY2BIT) {
    u8g.setColorIndex(3);  // max intensity
  } else if (u8g.getMode() == U8G_MODE_BW) {
    u8g.setColorIndex(1);  // pixel on
  } else if (u8g.getMode() == U8G_MODE_HICOLOR) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
  //===============================================================

  LoRa_last_millis = millis();

  Serial.println("Retrieving Data from EEPROM...");
  if (EEPROM.read(100) == true) {
    Serial.println("THIS DEVICE IS PAIRED");
    Serial.print("Slave Assignment:");
    Serial.println(EEPROM.read(101));

    DevicePaired = true;
    ThisDevice_SlaveAssignment = EEPROM.read(101);

    float fLat = 0.00f;
    Serial.print("GeoFence Midpoint Latitude:");
    Serial.println(EEPROM.get(0, fLat), 6);

    float fLon = 0.00f;
    Serial.print("GeoFence Midpoint Longitude:");
    Serial.println(EEPROM.get(20, fLon), 6);

    uint16_t uRad = 0;
    Serial.print("GeoFence Radius:");
    Serial.println(EEPROM.get(40, uRad));

    GeoFenceMid_Lat = fLat;
    GeoFenceMid_Lon = fLon;
    GeoFence_Rad = uRad;

  } else {
    Serial.println("THIS DEVICE IS NOT PAIRED");
  }
}

void loop() {
  BuzzAlarm();
  gps_loop();

  int packetSize = LoRa.parsePacket();
  if (packetSize)  // Only read if there is some data to read..
  {
    LoRa.readBytes((uint8_t *)&MasterPayload, packetSize);
    Serial.println("**********************************");
    Serial.print("Received packet:");
    Serial.write((uint8_t *)&MasterPayload, sizeof(MasterPayload));
    Serial.print("\nHeader0:");
    Serial.println(MasterPayload.header0, HEX);
    Serial.print("Header1:");
    Serial.println(MasterPayload.header1, HEX);
    Serial.println("Slave ID:" + String(MasterPayload.SlaveID));
    Serial.println("Slave PW:" + String(MasterPayload.SlavePW));
    Serial.print("Slave Assignment:");
    Serial.println(MasterPayload.SlaveAssignment);
    Serial.print("Footer:");
    Serial.println(MasterPayload.footer, HEX);
    Serial.println("\n**********************************");


    if (String(MasterPayload.SlaveID) == thisSlave_ID && String(MasterPayload.SlavePW) == thisSlave_PinCode) {
      Serial.println("SLAVE ID && PW CONFIRMED!");
      if (MasterPayload.PairCommand == true) {
        Serial.println("Pairing Command!");
        DevicePaired = true;
      } else if (MasterPayload.PairCommand == false) {
        Serial.println("Terminate Command!");
        DevicePaired = false;
        ReplyTerminate = true;
      }
      ThisDevice_SlaveAssignment = MasterPayload.SlaveAssignment;
      Serial.print("GeoMid Latitude:");
      Serial.println(MasterPayload.GeofenceMidpoint_Lat, 6);
      Serial.print("GeoMid Longitude:");
      Serial.println(MasterPayload.GeofenceMidpoint_Lon, 6);
      Serial.print("GeoFence Radius:");
      Serial.println(MasterPayload.GeofenceRad);

      GeoFenceMid_Lat = MasterPayload.GeofenceMidpoint_Lat;
      GeoFenceMid_Lon = MasterPayload.GeofenceMidpoint_Lon;
      GeoFence_Rad = MasterPayload.GeofenceRad;
      Serial.println("##############################################");
      EEPROM.put(0, GeoFenceMid_Lat);
      EEPROM.put(20, GeoFenceMid_Lon);
      EEPROM.put(40, GeoFence_Rad);
      EEPROM.write(100, DevicePaired);
      EEPROM.write(101, ThisDevice_SlaveAssignment);
      Serial.println("##############################################");
    }
  }


  //=============== SEND PACKET =====================
  if (DevicePaired && (millis() - LoRa_last_millis) > LoRa_SendInterval) {
    // send packet
    Serial.println("Sending LoRa Packet");
    SlavePayload.count = iter;
    SlavePayload.SlaveAssignment = ThisDevice_SlaveAssignment;

    LoRa.beginPacket();
    LoRa.write((uint8_t *)&SlavePayload, sizeof(SlavePayload));
    LoRa.endPacket();

    LoRa_last_millis = millis();
    iter++;
    Serial.print("SLAVE DISTANCE FROM MIDPOINT:");
    Serial.print(DistanceFrom_GeoFenceMidpoint);
    Serial.println(" meters");
  }
  //=================================================
  DistanceFrom_GeoFenceMidpoint = calculateDistance(GeoFenceMid_Lat, GeoFenceMid_Lon, GPS_Lat, GPS_Lon);
  //reply to terminate
  if (ReplyTerminate) {
    Serial.println("Sending LoRa Packet");
    SlavePayload.SlaveAssignment = 100;

    LoRa.beginPacket();
    LoRa.write((uint8_t *)&SlavePayload, sizeof(SlavePayload));
    LoRa.endPacket();
    delay(500);
    LoRa.beginPacket();
    LoRa.write((uint8_t *)&SlavePayload, sizeof(SlavePayload));
    LoRa.endPacket();
    delay(500);
    LoRa.beginPacket();
    LoRa.write((uint8_t *)&SlavePayload, sizeof(SlavePayload));
    LoRa.endPacket();
    delay(500);
    ReplyTerminate = false;
  }


  //============================ OLED =============================
  char ch_GPS_Lat[20] = { ' ' };
  int int_GPS_Lat = (int)GPS_Lat;
  float f_GPS_lat = (abs(GPS_Lat) - abs(int_GPS_Lat)) * 100000;
  int fra_GPS_lat = (int)f_GPS_lat;
  sprintf(ch_GPS_Lat, "%d.%d", int_GPS_Lat, fra_GPS_lat);

  char ch_GPS_Lon[20] = { ' ' };
  int int_GPS_Lon = (int)GPS_Lon;
  float f_GPS_Lon = (abs(GPS_Lon) - abs(int_GPS_Lon)) * 100000;
  int fra_GPS_Lon = (int)f_GPS_Lon;
  sprintf(ch_GPS_Lon, "%d.%d", int_GPS_Lon, fra_GPS_Lon);

  char ch_DistanceFrom_GeoFenceMidpoint[20] = { ' ' };
  int int_DistanceFrom_GeoFenceMidpoint = (int)DistanceFrom_GeoFenceMidpoint;
  float f_DistanceFrom_GeoFenceMidpoint = (abs(DistanceFrom_GeoFenceMidpoint) - abs(int_DistanceFrom_GeoFenceMidpoint)) * 100000;
  int fra_DistanceFrom_GeoFenceMidpoint = (int)f_DistanceFrom_GeoFenceMidpoint;
  sprintf(ch_DistanceFrom_GeoFenceMidpoint, "%d.%d", int_DistanceFrom_GeoFenceMidpoint, fra_DistanceFrom_GeoFenceMidpoint);

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_helvR08);
    u8g.drawStr(0, 10, "GPS Loc. Valid:");
    if (valid_location) {
      u8g.drawStr(75, 10, "TRUE");
    } else {
      u8g.drawStr(75, 10, "FALSE");
    }
    u8g.drawStr(0, 23, "GPS Lat:");
    u8g.drawStr(50, 23, ch_GPS_Lat);
    u8g.drawStr(0, 36, "GPS Lon:");
    u8g.drawStr(50, 36, ch_GPS_Lon);

    u8g.drawStr(0, 49, "Dist f GeoMid:");
    u8g.drawStr(75, 49, ch_DistanceFrom_GeoFenceMidpoint);

  } while (u8g.nextPage());
  //===============================================================
}

float calculateDistance(float lat1, float long1, float lat2, float long2) {
  float dist;
  dist = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(long1 - long2);
  dist = acos(dist);
  dist = (6371 * pie * dist) / 180;
  return dist * 1000;
}
