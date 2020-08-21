#ifndef DISPLAY_H
#define DISPLAY_H

#include "MenuNavigate.h"
#include <LiquidCrystal_PCF8574.h> //Monochrome LCD based on PCF8574 drivers
#include <Adafruit_SSD1306.h> //Monochrome OLEDs based on SSD1306 drivers

class MenuDisplayPCF8574 : public IMenuDisplay {
  #define LCD_COLS 20
  #define LCD_ROWS 4
  #define DATA_FIELD_COL 14
  private:
    LiquidCrystal_PCF8574 lcd;
    uint8_t curLine;
    uint8_t prevLine;

  public:
    MenuDisplayPCF8574() : lcd(LCD_DSPL_ADDR) {
      curLine = 0;
      prevLine = 0;
    }

    //IMenuDisplay interface
    uint8_t getRowCount() {
      return LCD_ROWS;
    }
    
    void beginDraw() {
      lcd.clear();
      lcd.noBlink();
      curLine = 0;
    }
    
    void endDraw() {
    }

    void DrawTitle(MenuItemBase* item) {
      String title = item->GetTitle();
      uint8_t pos = (LCD_COLS - title.length()) / 2;
      if (pos < 0)
        pos = 0;
      lcd.setCursor(pos, curLine++);
      lcd.print(title);
    }
    
    void DrawItem(MenuItemBase* item) {
      String title = item->GetTitle();
      lcd.setCursor(1, curLine);
      lcd.print(title);
      
      String strData = item->GetDataAsString();
      if (strData.length() > 0) {
        strData = (strData + "     ").substring(0, LCD_COLS - DATA_FIELD_COL);
        lcd.setCursor(DATA_FIELD_COL, curLine);
        lcd.print(strData);
      }
      curLine++;
    }
    
    void selectLine(uint8_t line) {
      lcd.setCursor(0, prevLine);
      lcd.print(" ");
      lcd.setCursor(0, line);
      lcd.print(">");
      prevLine = line;
    }


    //Over interface
    void begin() {
      lcd.begin(LCD_COLS, LCD_ROWS);
      lcd.home();
    }

    void setBacklight(int brightness) {
      lcd.setBacklight(brightness);
    }
    
    void clear() {
      lcd.clear();
      lcd.noBlink();
    }
    
    void DrawField(MenuItemBase* item, bool blink = false) {
      String strData = item->GetDataAsString();
      if (strData.length() > 0) {
        strData = (strData + "     ").substring(0, LCD_COLS - DATA_FIELD_COL);
        lcd.setCursor(DATA_FIELD_COL, item->displayLine);
        lcd.print(strData);
      }
        
      if (blink) {
        lcd.setCursor(DATA_FIELD_COL - 1, item->displayLine);
        lcd.blink();
      }
      else
        lcd.noBlink();
    }
};


class MenuDisplaySSD1306 : public IMenuDisplay {
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define DATA_FIELD_POS 90
  #define TEXT_SIZE 1
  private:
    Adafruit_SSD1306 display;
    uint8_t curLine;
    uint8_t prevLine;
    
    int getVPos(uint8_t line) {
      return line * (8*TEXT_SIZE);
    }

    int len2width(uint8_t len) {
      return len * (8*TEXT_SIZE);
    }

  public:
    MenuDisplaySSD1306() : display(SCREEN_WIDTH, SCREEN_HEIGHT) {
      curLine = 0;
      prevLine = 0;
    }
    
    //IMenuDisplay interface
    uint8_t getRowCount() {
      return SCREEN_HEIGHT / (8*TEXT_SIZE);
    }
    
    void beginDraw() {
      display.clearDisplay();
      curLine = 0;
    }
    
    void endDraw() {
      display.display();
      delay(1);
    }
    
    void DrawTitle(MenuItemBase* item) {
      String title = item->GetTitle();
      uint8_t x = (SCREEN_WIDTH - len2width(title.length())) / 2;
      if (x < 0)
        x = 0;
      display.setCursor(x, getVPos(curLine++));
      display.println(title);
    }
    
    void DrawItem(MenuItemBase* item) {
      String title = item->GetTitle();
      display.setCursor(8, getVPos(curLine));
      display.println(title);
      
      String strData = item->GetDataAsString();
      if (strData.length() > 0) {
        display.setCursor(DATA_FIELD_POS, getVPos(curLine));
        display.println(strData);
      }
      curLine++;
    }
    
    void selectLine(uint8_t line) {
      display.fillCircle(3, getVPos(prevLine)+4, 3, SSD1306_BLACK);
      display.fillCircle(3, getVPos(line)+4, 3, SSD1306_WHITE);
      prevLine = line;
      display.display();
      delay(1);
    }

    
    //Over interface
    void begin() {
      display.begin(SSD1306_SWITCHCAPVCC, OLED_DSPL_ADDR);
      display.clearDisplay();
      display.setTextSize(TEXT_SIZE);
      display.setTextColor(SSD1306_WHITE);
      display.display();
      delay(1);
    }

    void setBacklight(int brightness) {
      display.invertDisplay(brightness);
      display.display();
      delay(1);
    }
    
    void clear() {
      display.clearDisplay();
      display.display();
      delay(1);
    }
    
    void DrawField(MenuItemBase* item, bool blink = false) {
      String strData = item->GetDataAsString();
      if (strData.length() > 0) {
        int pos = getVPos(item->displayLine);
        display.fillRect(DATA_FIELD_POS, pos, SCREEN_WIDTH-DATA_FIELD_POS, getVPos(1), blink ? SSD1306_WHITE : SSD1306_BLACK);
        display.setTextColor(blink ? SSD1306_BLACK : SSD1306_WHITE);
        display.setCursor(DATA_FIELD_POS, pos);
        display.println(strData);
        display.display();
        display.setTextColor(SSD1306_WHITE);
        delay(1);
      }
    }
};

#endif
