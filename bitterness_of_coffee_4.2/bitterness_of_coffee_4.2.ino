//fully coded by fin! Well all but the bits marked chatgpt...
#include <bitmaps.h>
#include <hibernation.h>
#include <globals.h>

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
  oled.setTextSize(1); 
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
      ui.currentScreen = uiState::SCREEN_MENU;
      menuSystem();
      break;
    case 1:
      Serial.println("switch2 pressed");
      ui.currentScreen = uiState::SCREEN_HOME;
      home();
      break;
    case 2:
      Serial.println("rotary switch pressed");
      if (ui.currentScreen == uiState::SCREEN_MENU){
        currentSubmenuIndex = menu.lastSelected;
        ui.currentScreen = uiState::SCREEN_SUBMENU;
        subMenuSystem();
      } else if(ui.currentScreen == uiState::SCREEN_SUBMENU){
        subMenuState& activeSubmenu = submenus[currentSubmenuIndex];
        const char* selectedItem = activeSubmenu.subMenuItems[activeSubmenu.lastSelected];

        if(strcmp(selectedItem, "Back") == 0){
          ui.currentScreen = uiState::SCREEN_MENU;
          menuSystem();
        }else {
          //toggle flags
          if(strcmp(selectedItem, "Bluetooth") == 0){
            bluetoothEnabled = !bluetoothEnabled;
            Serial.printf("Bluetooth is now %s\n", bluetoothEnabled ? "ON" : "OFF");
          }else if(strcmp(selectedItem, "Sound FX") == 0){
            soundEffects = !soundEffects;
            Serial.printf("SoundFX is now %s\n", soundEffects ? "ON" : "OFF");
          }else if(strcmp(selectedItem, "Enable Shuffle") == 0){
            shuffleEnabled = true;
            Serial.println("shuffle is now enabled");
          }else if(strcmp(selectedItem, "Enable Disabled") == 0){
            shuffleEnabled = false;
            Serial.println("shuffle is now disabled");
          }
          subMenuSystem();
        }
      }
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

void updateMenuHand(int newPos){
  if(newPos == menu.lastSelected) return; //stops redrawing if not needed
  if(menu.lastSelected != -1){
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
      epaper.fillRect(100, 4 + (menu.lastSelected % 3) * 64, 100, 64, GxEPD_WHITE);
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
    drawVerticalText(menu.items[newPos], 4, 0, 20);
  }while (epaper.nextPage());

  menu.lastSelected = newPos;
}

void menuSystem(){
  if (ui.currentScreen == uiState::SCREEN_MENU){
    if (menu.currentPage == menu.lastPage) return;
    Serial.println("menuSystem running");
    epaper.clearScreen();
    epaper.setFullWindow();
    epaper.setTextColor(GxEPD_BLACK);
    epaper.fillScreen(GxEPD_WHITE);
    epaper.firstPage();
    do{
        epaper.drawLine(30-6, 0, 30-6, 200, GxEPD_BLACK);

        if (menu.currentPage == 1){
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
    menu.lastSelected = 1;
    oldPos = 0;
    updateMenuHand(0);
  }
}

void subMenuSystem(){ //thx chatgpt 
  if (ui.currentScreen != uiState::SCREEN_SUBMENU) return;

  subMenuState& activeSubmenu = submenus[currentSubmenuIndex];
  
  epaper.setFullWindow();
  epaper.firstPage();
  do {
    epaper.fillScreen(GxEPD_WHITE);

    //divider
    epaper.drawLine(30-6, 0, 30-6, 200, GxEPD_BLACK);

    epaper.setTextSize(2);
    for (int i = 0; i < activeSubmenu.length; i++){
      int y = 20 + i * 30;
      epaper.setTextColor(GxEPD_BLACK);
      epaper.setCursor(40, y);
      epaper.print(activeSubmenu.subMenuItems[i]);
    }
    drawVerticalText(menu.items[currentSubmenuIndex], 4, 0, 20);
  }while (epaper.nextPage());

  invertSubmenu(activeSubmenu.lastSelected, true);
  oldPos = activeSubmenu.lastSelected;
}

void invertSubmenu(int index, bool highlight){
  int y = 20 + index * 30 - 5;
  int w = 160;
  int h = 25;

  epaper.setPartialWindow(30, y, w, h);
  epaper.firstPage();
  do {
    if (highlight) {
      epaper.fillRect(30, y, w, h, GxEPD_BLACK);
      epaper.setTextColor(GxEPD_WHITE);
    } else {
      epaper.fillRect(30, y, w, h, GxEPD_WHITE);
      epaper.setTextColor(GxEPD_BLACK);
    }
    epaper.setCursor(40, y + 5);
    epaper.print(submenus[currentSubmenuIndex].subMenuItems[index]);
  } while (epaper.nextPage());
}

void loop() {
  
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
          ui.lastActivityTime = millis();

          if (ui.isHibernateing){
            ui.isHibernateing = false;
            hibernation(); //if btn pressed and is hibernating kicks you out of hibernation
          } else{
            makeBtnDo(i);
          }
        }
      }
    }
    lastBtnState[i] = reading; //this caused so much wasted time because i forgot it :()
  } 
  hibernationTimeout(); //checks for timeout
  //rotary check
  if (ui.currentScreen == uiState::SCREEN_MENU){
    int newPos = ((enc.getCount() % menu.length) + menu.length) % menu.length; //+0 to +5 //+6%6; removes negaive numbers
    int newPage = newPos / 3;
    if (newPage != menu.currentPage){
      Serial.printf("page change from %d to %d\n", menu.currentPage, newPage);
      menu.lastPage = menu.currentPage;
      menu.currentPage = newPage;
      menuSystem();
    }

    if (newPos != oldPos){
      updateMenuHand(newPos);
      oldPos = newPos;
    }
  }

  if (ui.currentScreen == uiState::SCREEN_SUBMENU){
    subMenuState& submenu = submenus[currentSubmenuIndex];
    int newPos = ((enc.getCount() % submenu.length) + submenu.length) % submenu.length;
    if (newPos != oldPos){
      invertSubmenu(oldPos, false);
      invertSubmenu(newPos, true);
      submenu.lastSelected = newPos;
      oldPos = newPos;
    }
  }
}