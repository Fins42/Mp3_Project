#pragma once

#include <GxEPD2_BW.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>
#include <ctype.h>
#include <Wire.h>

//setup display (epaper) scl=18 sda=23 (misslabled on board)
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
  const char** settingsMenuItems;
  const int settingsMenuLength;
  int currentPage;
  int lastPage;
  int lastSelected;
};
extern subMenuState submenu;

struct uiState {
  enum screenState {
    SCREEN_PLAYLISTS,
    SCREEN_SHUFFLE,
    SCREEN_LIKED,
    SCREEN_SETTINGS,
    SCREEN_THEMES,
    SCREEN_ABOUT,
    SCREEN_HOME,
    SCREEN_MENU
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


//button stuff//
extern int btnstate[3];
extern int lastBtnState[3];
extern unsigned long lastDebounceTime[3];
extern unsigned long debounceDelay;

