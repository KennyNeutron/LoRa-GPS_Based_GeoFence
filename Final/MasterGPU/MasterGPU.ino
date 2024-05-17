#include "Arduino.h"
#include <SPI.h>
#include <SD.h>           // SD Card Library
#include <LCDWIKI_GUI.h>  //Core graphics library
#include <LCDWIKI_KBV.h>  //Hardware-specific library
#include <XPT2046_Touchscreen.h>


#include <EEPROM.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "variables.h"

#define TCS_PIN 53

XPT2046_Touchscreen ts(TCS_PIN);
//#define TIRQ_PIN 44
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
//XPT2046_Touchscreen ts(TCS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
//the definiens of 8bit mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV tft(NT35510, 40, 38, 39, 43, 41);  //model,cs,cd,wr,rd,reset


/*  r     g    b */
#define BLACK 0x0000       /*   0,   0,   0 */
#define BLUE 0x001F        /*   0,   0, 255 */
#define RED 0xF800         /* 255,   0,   0 */
#define GREEN 0x07E0       /*   0, 255,   0 */
#define CYAN 0x07FF        /*   0, 255, 255 */
#define MAGENTA 0xF81F     /* 255,   0, 255 */
#define YELLOW 0xFFE0      /* 255, 255,   0 */
#define WHITE 0xFFFF       /* 255, 255, 255 */
#define NAVY 0x000F        /*   0,   0, 128 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define ORANGE 0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define PINK 0xF81F        /* 255,   0, 255 */

#define BluetoothStatusPin 19

SoftwareSerial MasterSerial(10, 11);

uint16_t currentScreen = 0;

int dataIIC = 0;

bool I2CRecieved = false;

void setup() {
  Serial.begin(9600);
  MasterSerial.begin(9600);
  Serial.println("GeoFence Master GPU \nSystem Starting...");

  tft.Init_LCD();
  tft.Set_Rotation(1);

  ts.begin();
  ts.setRotation(3);

  if (!SD.begin(48)) {
    tft.Set_Text_Back_colour(BLUE);
    tft.Set_Text_colour(WHITE);
    tft.Set_Text_Size(1);
    tft.Print_String("SD Card Init fail!", 0, 0);
  }

  // tft.Fill_Screen(RED);
  // tft.Fill_Screen(GREEN);
  // tft.Fill_Screen(BLUE);

  // tft.Fill_Screen(BLACK);
  // display_LoadingScreen();
  // delay(2000);

  currentScreen = Screen_Main;
  Wire.begin(9);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);

  float fLat = 0.00f;
  Serial.print("GeoFence Midpoint Latitude:");
  Serial.println(EEPROM.get(0, fLat), 6);

  float fLon = 0.00f;
  Serial.print("GeoFence Midpoint Longitude:");
  Serial.println(EEPROM.get(20, fLon), 6);

  uint16_t uRad = 0;
  Serial.print("GeoFence Radius:");
  Serial.println(EEPROM.get(40, uRad));

  GeoFenceMidPoint_Latitude = fLat;
  GeoFenceMidPoint_Longitude = fLon;
  GeoFenceRadius = uRad;

  for (int x = 1; x <= 6; x++) {
    SlaveStatus[x] = EEPROM.read(x + 100);
    Serial.println("STATUS Slave [" + String(x) + "]:" + String(SlaveStatus[x]));
  }
  last_millis = millis();

  pinMode(BluetoothStatusPin, INPUT);
}


void receiveEvent(int bytes) {
  uint8_t count = 0;
  while (Wire.available())  // loop through data received
  {
    char c = Wire.read();  // receive byte as a byte
    // Serial.print("I2C DATA[" + String(count) + "]: 0x");
    Serial.println(c, HEX);
    ch_GeoSerialDATA[count] = c;
    count++;
  }
  I2CRecieved = true;
  last_millis = millis();
}
void loop() {
  switch (currentScreen) {
    case Screen_Main:
      display_Main();
      break;
    default:
      display_Main();
      break;
  }

  if (I2CRecieved == true) {
    switch (ch_GeoSerialDATA[0]) {
      case 0xFFFFFFAA:
        DecodeGeofenceData();
        SaveToEEPROM();
        break;
      case 0xFFFFFFAB:
        GeoFenceMidPoint_Latitude = 0.00;
        GeoFenceMidPoint_Longitude = 0.00;
        GeoFenceRadius = 0;
        SaveToEEPROM();
        break;
      case 0xFFFFFFBB:
        SlaveMeddle[ch_GeoSerialDATA[1]] = true;
        MeddledOnly[ch_GeoSerialDATA[1]] = false;
        AlarmType[ch_GeoSerialDATA[1]] = ch_GeoSerialDATA[2];
        break;
      case 0xFFFFFFCC:
        MeddledOnly[ch_GeoSerialDATA[1]] = true;
        break;
      default:
        delay(10);
        break;
    }
    I2CRecieved = false;
  }

  if (millis() - last_millis >= 10000) {
    ResetMeddle();
    last_millis = millis();
  }
}

void ResetMeddle() {
  for (int i = 0; i < 7; i++) {
    SlaveMeddle[i] = false;
    AlarmType[i] = false;
    MeddledOnly[i] = false;
  }
}

void SaveToEEPROM() {
  Serial.println("##############################################");
  EEPROM.put(0, GeoFenceMidPoint_Latitude);
  EEPROM.put(20, GeoFenceMidPoint_Longitude);
  EEPROM.put(40, GeoFenceRadius);
  Serial.println("##############################################");
}

void display_LoadingScreen() {
  tft.Fill_Screen(BLACK);
  show_string("PaliBOT --- GeoFence Device", CENTER, 200, 4, WHITE, WHITE, 0);
  show_string("STARTING...", CENTER, 350, 2, WHITE, WHITE, 0);
}

void show_string(String str, int16_t x, int16_t y, uint8_t csize, uint16_t fc, uint16_t bc, boolean mode) {
  tft.Set_Text_Mode(mode);
  tft.Set_Text_Size(csize);
  tft.Set_Text_colour(fc);
  tft.Set_Text_Back_colour(bc);
  tft.Print_String(str, x, y);
}

bool getBluetoothStatus() {
  return digitalRead(BluetoothStatusPin);
}
