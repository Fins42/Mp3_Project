#include "globals.h"

//EPAPER din=18 clk=23 
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

subMenuState submenus[6] = { // lil help from chatgpt thx :)
    //playlists
  { {"Playlist 1", "Playlist 2", "Back"},
    { nullptr, nullptr, nullptr },
    3, 0, -1, -1 },
    //shuffle
  { {"Shuffle", "Back"},
    { &shuffleEnabled, nullptr },
    2, 0, -1, -1 },
    //liked
  { {"Track 1", "Track 2", "Track 3", "Back"},
    { nullptr, nullptr, nullptr, nullptr },
    4, 0, -1, -1 },
    //settings
  { {"Bluetooth", "Sound FX", "Reset", "Back"},
    { &bluetoothEnabled, &soundEffects, nullptr, nullptr },
    4, 0, -1, -1 },
    //theme
  { {"Dark", "Light", "Back"},
    { &darkMode, &darkMode, nullptr }, // both share same bool
    3, 0, -1, -1 },
    //about
  { {"v4.2", "Credits", "Back"},
    { nullptr, nullptr, nullptr },
    3, 0, -1, -1 }
};


int currentSubmenuIndex = 0; 

uiState ui = {
  ui.SCREEN_HOME,
  false,
  0,
  5UL * 60 * 1000 //5-mins 
};

// IO PINS
const int volumepin = 34;
const int btnpins[3] = {12, 13, 14};
const int btnnum = 3;

//CHANGEABLE BOOLS
bool shuffleEnabled = false;
bool soundEffects = true;
bool bluetoothEnabled = false;
bool darkMode = false;

//BTN STUFF
int btnstate[3] = {0};
int lastBtnState[3] = {0};
unsigned long lastDebounceTime[3] = {0};
unsigned long debounceDelay = 50;