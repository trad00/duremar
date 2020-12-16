#include <Button2.h>
#include <ESPRotary.h>
#include <WiFiManager.h>

#include "consts.h"
#include "pitches.h"
#include "common.h"
#include "Config.h"
#include "AMain.h"
#include "WebServer.h"

enum MODES { MAIN, CONFIG };
MODES mode = MAIN;
//MODES mode = CONFIG;

Button2 button(ENC_BTN);
ESPRotary rotary(ENC_A, ENC_B, 4);

void onEncLeft(ESPRotary& rotary) {
  if (mode == CONFIG)
    config::onEncLeft(rotary);
  else {
    main::prevPage();
  }
}

void onEncRight(ESPRotary& rotary) {
  if (mode == CONFIG)
    config::onEncRight(rotary);
  else {
    main::nextPage();
  }
}

void buttonPressed(Button2& btn) {
  if (main::alarm) {
    main::alarm = false;
    return;
  }
  if (mode == CONFIG)
    config::buttonPressed(btn);
  else if (main::page == main::pageScale) {
    main::scaleTare();
  } else {
    mode = CONFIG;
    config::begin();
  }
}

void onConfigExit() {
  mode = MAIN;
  main::init();
  main::begin();
  main::draw(true);
}

void startWiFi() {
  config::enWIFIMODE wifiMode = config::conf["wifiMode"];
  if (wifiMode == config::AP) {
    WiFi.softAP(APSSID, APPSK);
    webServerIP = WiFi.softAPIP();
  }
  else {
    main::displStartWiFi();
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(30);
    if (wifiManager.autoConnect())
      webServerIP = WiFi.localIP();
    else
      webServerIP = WiFi.softAPIP();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

//  pinMode(RELAY1, OUTPUT);
//  pinMode(RELAY2, OUTPUT);
//  pinMode(RELAY3, OUTPUT);
//  pinMode(RELAY4, OUTPUT);
//  digitalWrite(RELAY1, REL_OFF);
//  digitalWrite(RELAY2, REL_OFF);
//  digitalWrite(RELAY3, REL_OFF);
//  digitalWrite(RELAY4, REL_OFF);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, LOW);

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BTN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  button.setPressedHandler(buttonPressed);
  rotary.setRightRotationHandler(onEncRight);
  rotary.setLeftRotationHandler(onEncLeft);

  config::setup(BUZZER, onConfigExit, main::resetScale);

  main::setup();
  main::init();
  main::welcome(config::conf["melodyOn"]);
  startWiFi();
  main::begin();
  
  if (mode == CONFIG)
    config::begin();
  else
    main::draw(true);

  webserver::setup();
  webserver::begin();

}

unsigned long lastRedrawTimer = 0;
unsigned long redrawIntervalTimer = 60000;
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
  webserver::loop();
}
