#ifndef DISPLAY_H
#define DISPLAY_H

#include "MenuNavigate.h"
#include <LiquidCrystal_PCF8574.h>

class MenuDisplayPCF8574 : public IMenuDisplay {
  #define LCD_COLS 20
  #define LCD_ROWS 4
  #define DATA_FIELD_COL 14
  private:
    LiquidCrystal_PCF8574 lcd;
    uint8_t curLine;
    uint8_t prevLine;

  public:
    MenuDisplayPCF8574() : lcd(DSPL_ADDR) {
      lcd.begin(LCD_COLS, LCD_ROWS);
      lcd.home();
      curLine = 0;
      prevLine = 0;
    }
    void setBacklight(int brightness) {
      lcd.setBacklight(brightness);
    }
    uint8_t getRowCount() {
      return LCD_ROWS;
    }
    void clear() {
      lcd.clear();
      lcd.noBlink();
    }
    void startDraw() {
      lcd.clear();
      lcd.noBlink();
      curLine = 0;
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
    void selectLine(uint8_t line) {
      lcd.setCursor(0, prevLine);
      lcd.print(" ");
      lcd.setCursor(0, line);
      lcd.print(">");
      prevLine = line;
    }
};

#endif
