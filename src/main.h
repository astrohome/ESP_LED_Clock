#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <Arduino.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ArduinoJson.h>

#include "global.h"
#include "NTP.h"

extern "C" {
#include "user_interface.h"
}
// Include the HTML, STYLE and Script "Pages"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "Page_NetworkConfiguration.h"
#include "Fonts.h"

// --------------- define
#define MAX_DEVICES 4 
#define CLK_PIN     D5 // or SCK
#define DATA_PIN    D7 // or MOSI
#define CS_PIN      D8 // or SS
#define LED_PIN 2
#define buttonPin 0
#define analogPIN A0

#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

// Global data
typedef struct
{
  textEffect_t  effect;   // text effect to display
  char *        psz;      // text string nul terminated
  uint16_t      speed;    // speed multiplier of library default
  uint16_t      pause;    // pause multiplier for library default
} sCatalog;

sCatalog  catalog[] =
{
  { PA_SLICE, (char *)"SLICE", 1, 1 },
  { PA_MESH, (char *)"MESH", 10, 1 },
  { PA_FADE, (char *)"FADE", 20, 1 },
  { PA_WIPE, (char *)"WIPE", 5, 1 },
  { PA_WIPE_CURSOR, (char *)"WPE_C", 4, 1 },
  { PA_OPENING, (char *)"OPEN", 3, 1 },
  { PA_OPENING_CURSOR, (char *)"OPN_C", 4, 1 },
  { PA_CLOSING, (char *)"CLOSE", 3, 1 },
  { PA_CLOSING_CURSOR, (char *)"CLS_C", 4, 1 },
  { PA_BLINDS, (char *)"BLIND", 7, 1 },
  { PA_DISSOLVE, (char *)"DSLVE", 7, 1 },
  { PA_SCROLL_UP, (char *)"SC_U", 5, 1 },
  { PA_SCROLL_DOWN, (char *)"SC_D", 5, 1 },
  { PA_SCROLL_LEFT, (char *)"SC_L", 5, 1 },
  { PA_SCROLL_RIGHT, (char *)"SC_R", 5, 1 },
  { PA_SCROLL_UP_LEFT, (char *)"SC_UL", 7, 1 },
  { PA_SCROLL_UP_RIGHT, (char *)"SC_UR", 7, 1 },
  { PA_SCROLL_DOWN_LEFT, (char *)"SC_DL", 7, 1 },
  { PA_SCROLL_DOWN_RIGHT, (char *)"SC_DR", 7, 1 },
  { PA_SCAN_HORIZ, (char *)"SCANH", 4, 1 },
  { PA_SCAN_VERT, (char *)"SCANV", 3, 1 },
  { PA_GROW_UP, (char *)"GRW_U", 7, 1 },
  { PA_GROW_DOWN, (char *)"GRW_D", 7, 1 },
};

// ------------------- objects
Ticker ticker;
os_timer_t myTimer;
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
WiFiClient client;

// ------------------- const
const char *weatherHostz = "api.openweathermap.org";

// ------------------- prototypes
void scrollIP();
void scrollConnect();
String utf8rus(String);
void tvoday(String);
void getWeatherDataz();
void getWeatherData();
void scrollText();
void scrollText1();
void scrollText2();
void scrollText3();
void displayInfo();
void displayInfo1();
void displayInfo2();
void displayInfo2();
void getTime();
void resetAll();