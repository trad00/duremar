#ifndef MAINDISPLAY_H
#define MAINDISPLAY_H

#include <LiquidCrystal_PCF8574.h> //Monochrome LCD based on PCF8574 drivers
#include <Adafruit_SSD1306.h> //Monochrome OLEDs based on SSD1306 drivers
#include "IPAddress.h"

IPAddress webServerIP;

#ifdef LCDDisplay

class MainDisplayPCF8574 {
#define LCD_COLS 20
#define LCD_ROWS 4
private:
  LiquidCrystal_PCF8574 lcd;
    
public:
  MainDisplayPCF8574() : lcd(LCD_DSPL_ADDR) {
  }
  
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
    
  void beginDraw() {
  }
  
  void endDraw() {
  }
  
  void drawTemp(uint8_t i, String temp) {
    lcd.setCursor(0, i);
    lcd.print("T" + String(i+1));
    lcd.setCursor(3, i);
    lcd.print(temp);
  }
  
  void drawRelay(uint8_t i, bool relOn) {
    lcd.setCursor(13, i);
    lcd.print("R" + String(i+1));
    lcd.setCursor(16, i);
    lcd.print(relOn ? "ON ": "OFF");
  }
  
  void drawPage2(String AtmT, String AtmP, String Alt) {
    lcd.setCursor(0, 0);
    lcd.print("Atm T ");
    lcd.print(AtmT);
    
    lcd.setCursor(0, 1);
    lcd.print("Atm P ");
    lcd.print(AtmP);
    
    lcd.setCursor(0, 2);
    lcd.print("Alt   ");
    lcd.print(Alt);
    
    lcd.setCursor(0, 3);
    lcd.print("IP: ");
    lcd.print(webServerIP);
  }
  
  void welcome() {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Duremar 4");
    lcd.setCursor(0, 1);
    lcd.print("Hello from Valerich!");
    lcd.setCursor(0, 2);
    lcd.print("(c) trad00@yandex.ru");
    lcd.setCursor(4, 3);
    lcd.print(VER);
  }
  
  void startWiFi() {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Starting WiFi ...");
  }
};

#else

class MainDisplaySSD1306 {
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DATA_FIELD_POS 90
#define TEXT_SIZE 1
private:
  Adafruit_SSD1306 display;

  int getVPos(uint8_t line) {
    return line * (8*TEXT_SIZE);
  }

  int len2width(uint8_t len) {
    return len * (8*TEXT_SIZE);
  }

public:
  MainDisplaySSD1306() : display(SCREEN_WIDTH, SCREEN_HEIGHT) {
  }
  
  void begin() {
    display.begin(SSD1306_SWITCHCAPVCC, OLED_DSPL_ADDR);
    display.display();
    delay(1);
    display.clearDisplay();
    display.setTextSize(TEXT_SIZE);
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);
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
//    display.display();
//    delay(1);
  }
  
  void beginDraw() {
    display.clearDisplay();
  }
  
  void endDraw() {
    display.display();
    delay(1);
  }
  
  void drawTemp(uint8_t i, String temp) {
    String str = "T" + String(i+1);
    display.setTextSize(2);
    display.setCursor(0, i*16);
    display.print(str);
    display.setCursor(30, i*16);
    display.print(temp);
  }
  
  void drawRelay(uint8_t i, bool relOn) {
    display.setCursor(120, i*16);
    if (relOn)
      display.fillRect(120, i*16+2, 8, 12, SSD1306_WHITE);
    else
      display.drawRect(120, i*16+2, 8, 12, SSD1306_WHITE);
  }
  
  void drawPage2(String AtmT, String AtmP, String Alt) {
    display.setTextSize(1);
    
    display.setCursor(0, 0);
    display.print("IP: ");
    display.print(webServerIP);

    display.setCursor(0, 16);
    display.print("Atm T ");
    display.print(AtmT);
    
    display.setCursor(0, 32);
    display.print("Atm P ");
    display.print(AtmP);
    
    display.setCursor(0, 48);
    display.print("Alt   ");
    display.print(Alt);
  }
  
  void welcome() {
    display.clearDisplay();
    
    display.setTextSize(2);
    display.setCursor(12, 0);
    display.print("Duremar 4");
    
    display.setTextSize(1);
    display.setCursor(4, 20);
    display.print("Hello from Valerich!");
    
    display.setTextSize(1);
    display.setCursor(4, 36);
    display.print("(c) trad00@yandex.ru");
    
    display.setTextSize(1);
    display.setCursor(4, 52);
    display.print(VER);
    
    display.setTextSize(TEXT_SIZE);
    display.display();
    delay(1);
  }
  
  void startWiFi() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(4, 28);
    display.print("Starting WiFi ...");
    display.setTextSize(TEXT_SIZE);
    display.display();
    delay(1);
  }
};

#endif

#endif
