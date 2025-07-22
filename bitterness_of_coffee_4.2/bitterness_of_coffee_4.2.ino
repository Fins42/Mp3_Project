//fully coded by fin! Well all but the bits marked chatgpt...
#include <GxEPD2_BW.h>
#include <bitmaps.h>
#include <ESP32Encoder.h>
#include <ctype.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//setup display (epaper) scl=18 sda=23 (misslabled on board)
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> epaper(GxEPD2_154_D67(/*CS*/ 5, /*DC*/ 4, /*RST*/ 19, /*BUSY*/ 15)); // GDEH0154D67 200x200, SSD1681
//oled
Adafruit_SSD1306 oled(128, 32, &Wire, -1);

//encoder
ESP32Encoder enc;
long oldPos= -999; //ensures first readout happens

//settings menu
const char* menuItems[] = {"Playlists", "Shuffle", "Liked", "Settings","Themes", "About"};
const int menuLength = 6; 
int currentPage = 0;
int lastPage = -1;
int lastSelected = -1; //hand
enum screenState {
  SCREEN_HOME,
  SCREEN_MENU,
  SCREEN_PLAYLISTS,
  SCREEN_SHUFFLE,
  SCREEN_LIKED,
  SCREEN_SETTINGS,
  SCREEN_THEMES,
  SCREEN_ABOUT
};
screenState currentScreen = SCREEN_HOME;

//io pins
const int volumepin = 34;
const int btnpins[] = {12, 13, 14};
const int btnnum = 3;

//button stuff//
int btnstate[3] = {0};
int lastBtnState[3] = {0};
unsigned long lastDebounceTime[3] = {0};
unsigned long debounceDelay = 50;

//hibernation//
unsigned long lastActivityTime = 0;
const unsigned long hibernationDelay = 5UL * 60 * 1000; //5 mins
//const unsigned long hibernationDelay = 5UL * 1000; //5 secs testing
bool isHibernateing = false;

void setup() {
  Serial.begin(115200);
  //encoder setup
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  enc.attachSingleEdge(32, 33);
  enc.setCount(0);
  //initialise epaper display 
  epaper.init(115200);
  epaper.setRotation(3);
  bootAnimation();
  epaper.refresh();
  //initialise oled
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  //set btn pins to input
  for(int i=0;i<btnnum;i++){
    pinMode(btnpins[i], INPUT_PULLUP);
  }
  home();
  oled.display();
  oled.clearDisplay();
  oled.setTextSize(1);  // Text size
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(10, 10);
  oled.println("Goodnight World!");
  oled.display();
}

void bootAnimation(){
  const int dotX= 90, dotY = 100, dotRadius = 4;

  //starting up txt
  epaper.setFullWindow();
  epaper.firstPage();
  do{
    epaper.fillScreen(GxEPD_WHITE);
    epaper.setTextColor(GxEPD_BLACK);
    epaper.setTextSize(2);
    epaper.setCursor(28, 66);
    epaper.print("starting up");
  }while (epaper.nextPage());
  delay(100);

  // animated dots -thx chatgpt i could enver code this 
  for (int i = 0; i < 3; i++) {
    epaper.setPartialWindow(dotX - dotRadius - 2, dotY - dotRadius - 2, 3 * 15 + dotRadius * 2, dotRadius * 2 + 4);
    epaper.firstPage();
    do {
      epaper.fillScreen(GxEPD_WHITE);
      for (int j = 0; j < 3; j++) {
        int cx = dotX + j * 15;
        (j == i % 3) ? epaper.fillCircle(cx, dotY, dotRadius, GxEPD_BLACK)
                     : epaper.drawCircle(cx, dotY, dotRadius, GxEPD_BLACK);
      }
    } while (epaper.nextPage());
    delay(150);
  }
}

void makeBtnDo(int btnindex){
  switch (btnindex){
    case 0:
      Serial.println("switch1 pressed");
      currentScreen = SCREEN_MENU;
      menuSystem();
      break;
    case 1:
      Serial.println("switch2 pressed");
      currentScreen = SCREEN_HOME;
      home();
      break;
    case 2:
      Serial.println("rotary switch pressed");
      break;
    default:
      Serial.println("what are you even pressing??");
      break;
  }
}

void home(){
  epaper.setFullWindow();
  epaper.firstPage();
  do{
    epaper.clearScreen();
    delay(500);
    epaper.fillScreen(GxEPD_BLACK);
    delay(500);
    epaper.fillScreen(GxEPD_WHITE);
    epaper.drawLine(30-6, 0, 30-6, 200, GxEPD_BLACK);
    drawVerticalText("home", 4, 0 , 20);
    epaper.drawLine(24, 200-50, 200, 200-50, GxEPD_BLACK);

  }while (epaper.nextPage());
}

void drawVerticalText(const char* text, int16_t x, int16_t yStart, int16_t spacing) {
  int len = strlen(text);
  int totalHeight = len * spacing;
  int16_t y = yStart + (200 - totalHeight) / 2;
  epaper.setTextSize(2);
  for (int i = 0; text[i] != '\0'; i++) {
    epaper.setCursor(x, y);
    epaper.print((char)toupper(text[i]));
    y += spacing; // thank you again chatgpt im amazed
  }
}

void hibernation(){
  if(isHibernateing == true){
    epaper.setPartialWindow(170, 0, 30, 20);
    epaper.firstPage();
    do{
      epaper.setTextColor(GxEPD_BLACK);
      epaper.setCursor(172, 4);
      epaper.setTextSize(2);
      epaper.print("zz");
      epaper.hibernate();
      Serial.println("debug: entered hibernation");
    }while (epaper.nextPage());
    oled.ssd1306_command(SSD1306_DISPLAYOFF);
  }
  else{
    //clear zz
    epaper.setPartialWindow(170, 0, 30, 20);
    epaper.firstPage();
    do{
      epaper.fillRect(170, 0, 30, 20, GxEPD_WHITE);
    }while (epaper.nextPage());
    lastActivityTime = millis();
    Serial.println("debug: exited hibernation");
    oled.ssd1306_command(SSD1306_DISPLAYON);
  }
}

void menuSystem(){
  if (currentPage == lastPage) return;
  Serial.println("menuSystem running");
  epaper.clearScreen();
  epaper.setFullWindow();
  epaper.setTextColor(GxEPD_BLACK);
  epaper.fillScreen(GxEPD_WHITE);
  epaper.firstPage();
  do{
      epaper.drawLine(30-6, 0, 30-6, 200, GxEPD_BLACK);

      if (currentPage == 1){
        for (int i = 3; i < 6; i++){
          epaper.drawInvertedBitmap(30, (i - 3) * 64 +4, BM_allArray[i], 64, 64, GxEPD_BLACK);
        }
      }else{
        for (int i = 0; i < 3; i++){
         epaper.drawInvertedBitmap(30, i * 64 +4, BM_allArray[i], 64, 64, GxEPD_BLACK);
        }
      }
  }while (epaper.nextPage());

  //reset hand if page has been changed
  lastSelected = 1;
  oldPos = 0;
  updateMenuHand(0);
}

void updateMenuHand(int newPos){
  if(newPos == lastSelected) return; //stops redrawing if not needed

  if(lastSelected != -1){
    //erase  vertical text
    epaper.setPartialWindow(0, 0, 22, 200);
    epaper.firstPage();
    do{
      epaper.fillRect(6, 0, 30, 200, GxEPD_WHITE);
    }while (epaper.nextPage());
    //erase hand - bye bye thanks for all the fish
    epaper.setPartialWindow(100, 0, 100, 200);
    epaper.firstPage();
    do{
      epaper.fillRect(100, 4 + (lastSelected % 3) * 64, 100, 64, GxEPD_WHITE);
    }while (epaper.nextPage());
  }
  //draw hand
  epaper.setPartialWindow(100, 0, 100, 200);
  epaper.firstPage();
  do {
    epaper.drawInvertedBitmap(100, 4 +(newPos % 3) * 64, BM_hand, 100, 64, GxEPD_BLACK);
  }while (epaper.nextPage());

  //vertical text
  epaper.setPartialWindow(0, 0, 22, 200);
  epaper.firstPage();
  do{
    drawVerticalText(menuItems[newPos], 4, 0, 20);
  }while (epaper.nextPage());

  lastSelected = newPos;
}

void loop() {
  //hibernation
  for (int i = 0; i < btnnum; i++){
    int reading = digitalRead(btnpins[i]);
    if (reading != lastBtnState[i]){
      lastDebounceTime[i] = millis(); //resets debounce time
    }
    //check for debounce timeout
    if ((millis() - lastDebounceTime[i]) > debounceDelay){
      if (reading != btnstate[i]){
        btnstate[i] = reading;

        if (btnstate[i] == LOW){
          lastActivityTime = millis();

          if (isHibernateing){
            isHibernateing = false;
            hibernation(); //if btn pressed and is hibernating kicks you out of hibernation
          } else{
            makeBtnDo(i);
          }
        }
      }
    }
    lastBtnState[i] = reading; //this caused so much wasted time because i forgot it :()
  } 
  //check for hibernation timeout
  if(!isHibernateing && (millis() - lastActivityTime > hibernationDelay)){
    isHibernateing = true;
    hibernation();
  }

  //rotary check
  if (currentScreen == SCREEN_MENU){
    int newPos = ((enc.getCount() % menuLength) + menuLength) % menuLength; //+0 to +5 //+6%6; removes negaive numbers
    int newPage = newPos / 3;
    if (newPage != currentPage){
      Serial.printf("page change from %d to %d\n", currentPage, newPage);
      lastPage = currentPage;
      currentPage = newPage;
      menuSystem();
    }

    if (newPos != oldPos){
      updateMenuHand(newPos);
      oldPos = newPos;
    }
  }
}