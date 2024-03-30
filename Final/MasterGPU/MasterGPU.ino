#include "Arduino.h"
#include <SPI.h>
#include <SD.h>           // SD Card Library
#include <LCDWIKI_GUI.h>  //Core graphics library
#include <LCDWIKI_KBV.h>  //Hardware-specific library
#include <XPT2046_Touchscreen.h>

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


SoftwareSerial MasterSerial(10, 11);

uint16_t currentScreen = 0;


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

  if (MasterSerial.available() > 0) {
    Serial.println("RECIEVED");
    MasterSerialDATA = MasterSerial.read();
    Serial.println(MasterSerialDATA, DEC);

    if (MasterSerialDATA == 0xAA) {
      Serial.println("HEAD");
      MasterSerialDATA = MasterSerial.read();
      NumberOfBytesToRead = MasterSerialDATA;
      MasterSerialDATA = MasterSerial.read();
      if (MasterSerialDATA == 0xAB) {
        Serial.println("FOOT");
        Serial.println("Bytes to READ:" + String(NumberOfBytesToRead));
      }
    }
  }
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
