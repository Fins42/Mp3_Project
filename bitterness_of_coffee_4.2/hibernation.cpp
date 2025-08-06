#include "hibernation.h"

void hibernation(){
  if(ui.isHibernateing == true){
    epaper.setPartialWindow(170, 0, 30, 20);
    epaper.firstPage();
    do{
      epaper.setTextColor(GxEPD_BLACK);
      epaper.setCursor(172, 4);
      epaper.setTextSize(2);
      epaper.print("zz");
      Serial.println("debug: entered hibernation");
    }while (epaper.nextPage());
    oled.ssd1306_command(SSD1306_DISPLAYOFF);
    epaper.hibernate();
    delay(100);
  }
  else{
    //clear zz
    epaper.setPartialWindow(170, 0, 30, 20);
    epaper.firstPage();
    do{
      epaper.fillRect(170, 0, 30, 20, GxEPD_WHITE);
    }while (epaper.nextPage());

    switch(ui.currentScreen){
      case uiState::SCREEN_HOME:
        home();
        break;
      case uiState::SCREEN_MENU:
        menuSystem();
        break;
      case uiState::SCREEN_SUBMENU:
        subMenuSystem();
        break;
      default:
        home();
        Serial.println("what menu are you even on??");
        break;
    }
    ui.lastActivityTime = millis();
    Serial.println("debug: exited hibernation");
    oled.ssd1306_command(SSD1306_DISPLAYON);
  }
}

void hibernationTimeout(){
  //check for hibernation timeout
  if(!ui.isHibernateing && (millis() - ui.lastActivityTime > ui.hibernationDelay)){
    ui.isHibernateing = true;
    hibernation();
  }

}