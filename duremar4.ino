#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <LiquidCrystal_PCF8574.h>
#include <Button.h>
#include <ESPRotary.h>


#include "ConfigNavigate.h"
#define ENC_BTN D5
#define ENC_A D6
#define ENC_B D7

#define LCD_COLS 20
#define LCD_ROWS 4
LiquidCrystal_PCF8574 lcd(0x27);

Button button(ENC_BTN);
ESPRotary rotary(ENC_A, ENC_B, 4);
ConfigNavigate cn(ENC_A, ENC_B, ENC_BTN, lcd, LCD_ROWS);

void buttonPressed() {
  Serial.println("main buttonPressed");
  Serial.print("cn.started() ");
  Serial.println(cn.started());
  cn.begin();
}

void rotRight(ESPRotary& rotary) {
  Serial.println("main rotRight");
}

void rotLeft(ESPRotary& rotary) {
  Serial.println("main rotLeft");
}

void draw() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DUREMAR 4");
}

void onConfigExit() {
  draw();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.setBacklight(true);
  lcd.home();
  
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BTN, INPUT_PULLUP);
  button.begin();
  rotary.setRightRotationHandler(rotRight);
  rotary.setLeftRotationHandler(rotLeft);
  cn.setConfigExit(onConfigExit);
  
//  cn.begin();
  draw();
}

unsigned long lastUpdate = 0;
unsigned long updateInterval = 1000;

void loop() {
  if (cn.started()) {
    cn.loop();
  } else {
    rotary.loop();
    if (button.toggled())
      if (button.read() == Button::PRESSED)
        buttonPressed();
  }

  if ((millis() - lastUpdate >= updateInterval) || lastUpdate == 0) {
    lastUpdate = millis();
  }
}
