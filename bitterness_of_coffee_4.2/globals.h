#pragma once

#include <GxEPD2_BW.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>
#include <ctype.h>
#include <Wire.h>

//setup display (epaper) din=18 clk=23 
extern GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> epaper;

//oled
extern Adafruit_SSD1306 oled;

//encoder
extern ESP32Encoder enc;
extern long oldPos;

struct menuState{
  const char* items[6];
  const int length; 
  int currentPage;
  int lastPage;
  int lastSelected;
};
extern menuState menu;

struct subMenuState{
  const char* subMenuItems[6];
  const int length;
  int currentPage;
  int lastPage;
  int lastSelected;
};
extern subMenuState submenus[6];
extern int currentSubmenuIndex;

struct uiState {
  enum screenState {
    SCREEN_HOME,
    SCREEN_MENU,
    SCREEN_SUBMENU
  };
  screenState currentScreen;
  bool isHibernateing;
  unsigned long lastActivityTime;
  const unsigned long hibernationDelay; 
};
extern uiState ui;

//io pins
extern const int volumepin;
extern const int btnpins[3];
extern const int btnnum;

//chanagebale bools
extern bool shuffleEnabled;
extern bool soundEffects;
extern bool bluetoothEnabled;
extern bool darkMode;

//colour inversion macro -chatgpts help thx
#define COLOR(c) ((darkMode && ((c) == GxEPD_WHITE)) ? GxEPD_BLACK : \
                  (darkMode && ((c) == GxEPD_BLACK)) ? GxEPD_WHITE : (c))

//button stuff//
extern int btnstate[3];
extern int lastBtnState[3];
extern unsigned long lastDebounceTime[3];
extern unsigned long debounceDelay;

