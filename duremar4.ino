#include <Button2.h>
#include <ESPRotary.h>

#include "consts.h"
#include "pitches.h"
#include "common.h"
#include "Config.h"
#include "AMain.h"

enum MODES { MAIN, CONFIG };
MODES mode = MAIN;
//MODES mode = CONFIG;

Button2 button(ENC_BTN);
ESPRotary rotary(ENC_A, ENC_B, 4);

void onEncLeft(ESPRotary& rotary) {
  if (mode == CONFIG)
    config::onEncLeft(rotary);
  else {
    main::nextPage();
  }
}

void onEncRight(ESPRotary& rotary) {
  if (mode == CONFIG)
    config::onEncRight(rotary);
  else {
    main::prevPage();
  }
}

void buttonPressed(Button2& btn) {
  if (mode == CONFIG)
    config::buttonPressed(btn);
  else {
    mode = CONFIG;
    config::begin();
  }
}

void onConfigExit() {
  mode = MAIN;
  main::init();
  main::draw(true);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BTN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  digitalWrite(RELAY1, REL_OFF);
  digitalWrite(RELAY2, REL_OFF);
  digitalWrite(RELAY3, REL_OFF);
  digitalWrite(RELAY4, REL_OFF);

  button.setPressedHandler(buttonPressed);
  rotary.setRightRotationHandler(onEncRight);
  rotary.setLeftRotationHandler(onEncLeft);
  
  config::setup(BUZZER, onConfigExit);

  main::setup();
  main::init();
  main::welcome();
  main::begin();
  
  if (mode == CONFIG)
    config::begin();
  else
    main::draw(true);

//  if (config::conf["soundOn"])
//    startMelodyPlay();
}

unsigned long lastRedrawTimer = 0;
unsigned long redrawIntervalTimer = 10000;
bool getRedraw() {
  if ((millis() - lastRedrawTimer >= redrawIntervalTimer) || lastRedrawTimer == 0) {
    lastRedrawTimer = millis();
    //защита от сбоя экрана при помехах - перерисовка
    return true;
  }
  return false;
}

void loop() {
  rotary.loop();
  button.loop();
  if (mode == MAIN){
    main::loop();
    main::draw(getRedraw());
  }
}
