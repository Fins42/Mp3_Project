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