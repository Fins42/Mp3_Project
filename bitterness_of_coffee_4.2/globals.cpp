#include "globals.h"

//EPAPER
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> epaper(GxEPD2_154_D67(/*CS*/ 5, /*DC*/ 4, /*RST*/ 19, /*BUSY*/ 15));
//OLED
Adafruit_SSD1306 oled(128, 32, &Wire, -1);
//ENCODER
ESP32Encoder enc;
long oldPos = -999;

menuState menu = {
  {"Playlists", "Shuffle", "Liked", "Settings", "Themes", "About"},
  6,
  0,
  -1,
  -1
};

const char* settingsMenuItems[] = {"bluetoothEnabled", "Sound Effects", "Back"};
subMenuState submenu = {
  settingsMenuItems,
  3,
  0,
  -1,
  -1
};

uiState ui = {
  uiState::SCREEN_HOME,
  false,
  0,
  5UL * 60 * 1000
};

// IO PINS
const int volumepin = 34;
const int btnpins[3] = {12, 13, 14};
const int btnnum = 3;

//CHANGEABLE BOOLS
bool shuffleEnabled = false;
bool soundEffects = true;
bool bluetoothEnabled = false;

//BTN STUFF
int btnstate[3] = {0};
int lastBtnState[3] = {0};
unsigned long lastDebounceTime[3] = {0};
unsigned long debounceDelay = 50;