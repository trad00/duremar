#include <Button2.h>
#include <ESPRotary.h>

#include "consts.h"
#include "pitches.h"
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
    main::page = 1 - main::page;
    main::draw(true);
    main::doSound(100, 5);
  }
}

void onEncRight(ESPRotary& rotary) {
  if (mode == CONFIG)
    config::onEncRight(rotary);
  else {
    main::page = 1 - main::page;
    main::draw(true);
    main::doSound(100, 5);
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

  button.setPressedHandler(buttonPressed);
  rotary.setRightRotationHandler(onEncRight);
  rotary.setLeftRotationHandler(onEncLeft);
  
  config::setup(BUZZER, onConfigExit);

  main::setup();
  main::init();
  
  if (mode == CONFIG)
    config::begin();
  else
    main::draw(true);

//  if (config::conf["soundOn"])
//    startMelodyPlay();
}

void loop() {
  rotary.loop();
  button.loop();
  if (mode == MAIN){
    main::loop();
    main::draw();
  }
}
