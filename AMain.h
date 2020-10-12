#ifndef AMAIN_H
#define AMAIN_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP280.h>

#include "AMainDisplay.h"
#include "BitsToShift.h"

namespace main {

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_BMP280 bmp;

#ifdef LCDDisplay
MainDisplayPCF8574 disp;
#else
MainDisplaySSD1306 disp;
#endif

uint8_t page = 0;
uint8_t pinSound;
bool soundOn;
bool bmpBegan;

unsigned long lastUpdate = 0;
unsigned long updateInterval = 5000;

unsigned long lastUpdateTimer = 0;
unsigned long updateIntervalTimer = 1000;

unsigned long lastAlarmTimer = 0;
unsigned long alarmIntervalTimer = 1000;
bool alarm = false;

float _temps[4];
double _atm_t;
double _atm_prs_mmHg;
double _bar_alt;
bool _rOn[4];

float temps[4] = {emptySignal,emptySignal,emptySignal,emptySignal};
float clbTemps[4] = {0,0,0,0};
float atmCorr[4] = {0,0,0,0};
float normP = 760;
double atm_t = emptySignal;
double atm_prs_mmHg = emptySignal;
double bar_alt = emptySignal;

float getTempSensor(uint8_t tIndex) {
  float temp = temps[tIndex];
  if (temp != emptySignal) {
    temp += clbTemps[tIndex];
  }
  if (temp != emptySignal && atm_prs_mmHg != emptySignal) {
    temp += atmCorr[tIndex] * (atm_prs_mmHg - normP);
  }
  temp = roundPrec(temp, 1);
  return temp;
}

void setSound(uint8_t pin, bool on) {
  pinSound = pin;
  soundOn = pinSound >= 0 && on;
};

void doSound(uint16_t frequency, uint16_t duration) {
  if (soundOn)
    tone(pinSound, frequency, duration);
}

float roundFloat(float f, int prec) {
  int mul = pow(10, prec);
  return float(round(f * mul)) / mul;
}

String rightAlign(String str, uint8_t num) {
  String space;
  for (uint8_t i = str.length(); i < num; i++)
    space += " ";
  return space + str;
}

String fmtFloatValue(float val, int decPlc, String unit) {
  if (val == emptySignal){
    return "n/a";
  }
  uint8_t num = decPlc == 0 ? 3 : 4 + decPlc;
  return rightAlign(String(val, decPlc), num) + unit;
}

String fmtTemperature(float val, int decPlc) {
  String unit = "\xDF";
  unit += "C  ";
  return fmtFloatValue(val, decPlc, unit);
}

String fmtPressure(float val, int decPlc) {
  return fmtFloatValue(val, decPlc, " mmHg  ");
}

String fmtAlt(float val, int decPlc) {
  return fmtFloatValue(val, decPlc, " m  ");
}

BitsToShift bts;

class RelayActuator {
public:
  RelayActuator(uint8_t chan) {
    pref = String(chan);
    switch(chan) {
      case 1:
        reg_bit = 0b01000000;
//        pin = RELAY1;
        break;
      case 2:
        reg_bit = 0b00100000;
//        pin = RELAY2;
        break;
      case 3:
        reg_bit = 0b00010000;
//        pin = RELAY3;
        break;
      default:
        reg_bit = 0b10000000;
//        pin = RELAY4;
    }
  }
  
  void init() {
    relMode = config::conf[pref + "/mode"];
    soundAlarm = config::conf[pref + "/sound"];
    
    tOnSet = isSet(config::conf[pref + "/on/sensor"], tOnIndex);
    operOn = config::conf[pref + "/on/oper"];
    tempOn = config::conf[pref + "/on/temp"];
    timeMMOn = config::conf[pref + "/on/timeMM"];
    timeSSOn = config::conf[pref + "/on/timeSS"];
    
    tOffSet = isSet(config::conf[pref + "/off/sensor"], tOffIndex);
    operOff = config::conf[pref + "/off/oper"];
    tempOff = config::conf[pref + "/off/temp"];
    timeMMOff = config::conf[pref + "/off/timeMM"];
    timeSSOff = config::conf[pref + "/off/timeSS"];
  }
  
  config::enRELMODE getMode() {
    return relMode;
  }
  
  void setMode(config::enRELMODE mode) {
    relMode = mode;
    config::conf[pref + "/mode"] = relMode;
    config::saveConfig();
    actuate();
  }
  
  void shiftMode() {
    switch (relMode) {
      case config::AUTO:
        setMode(config::rmON);
        break;
      case config::rmON:
        setMode(config::rmOFF);
        alarm = false;
        break;
      default:
        setMode(config::AUTO);
    }
  }
  
  void actuate() {
    switch (relMode) {
      case config::rmON:
        ON();
//        digitalWrite(pin, REL_ON);
        break;
      case config::rmOFF:
        OFF();
//        digitalWrite(pin, REL_OFF);
        break;
      default: {
        if (tOnSet) {
          float temp = getTempSensor(tOnIndex);
          
          bool isMore = (temp - tempOn) > 0.001;
          bool isLess = (tempOn - temp) > 0.001;

          if (operOn == config::MORE && isMore || operOn == config::LESS && isLess) {
//            if (digitalRead(pin) == REL_OFF) {
//              digitalWrite(pin, REL_ON);
            if (!STATUS()) {
              ON();
              alarm = soundAlarm;
              timeOn = millis();
            }
          }
        }
        if (tOffSet) {
          float temp = getTempSensor(tOffIndex);

          bool isMore = (temp - tempOff) > 0.001;
          bool isLess = (tempOff - temp) > 0.001;
          
          if (operOff == config::MORE && isMore || operOff == config::LESS && isLess) {
//            if (digitalRead(pin) == REL_ON) {
//              digitalWrite(pin, REL_OFF);
            if (STATUS()) {
              OFF();
              alarm = false;
              timeOff = millis();
            }
          }
        }
      }
    }
  }
  
  void timerCheck() {
    if (tOnSet && (timeMMOn != 0 || timeSSOn != 0)) {
      if (millis()-timeOn > (timeMMOn*60 + timeSSOn)*1000) {
//        if (digitalRead(pin) == REL_ON) {
//          digitalWrite(pin, REL_OFF);
        if (STATUS()) {
          OFF();
          alarm = false;
        }
      }
    }
    if (tOffSet && (timeMMOff != 0 || timeSSOff != 0)) {
      if (millis()-timeOff > (timeMMOff*60 + timeSSOff)*1000) {
//        if (digitalRead(pin) == REL_OFF) {
//          digitalWrite(pin, REL_ON);
        if (!STATUS()) {
          ON();
          alarm = soundAlarm;
        }
      }
    }
  }
  
//  inline uint8_t getPin() {
//    return pin;
//  }
  
  inline bool STATUS() {
    return bts.getBit(reg_bit);
  }
  
  inline void ON() {
    if (!STATUS())
      bts.setBit(reg_bit, true);
  }
  
  inline void OFF() {
    if (STATUS())
      bts.setBit(reg_bit, false);
  }
  
private:
  bool isSet(config::enSENSNO no, uint8_t& tIndex) {
    switch(no) {
      case config::T1:
        tIndex = 0;
        return true;
      case config::T2:
        tIndex = 1;
        return true;
      case config::T3:
        tIndex = 2;
        return true;
      case config::T4:
        tIndex = 3;
        return true;
    }
    return false;
  }
  
private:
  config::enRELMODE relMode;

  bool tOnSet;
  uint8_t tOnIndex;
  config::enOPER operOn;
  float tempOn;
  unsigned long timeOn;
  
  bool tOffSet;
  uint8_t tOffIndex;
  config::enOPER operOff;
  float tempOff;
  unsigned long timeOff;
  
  int timeMMOn;
  int timeSSOn;
  int timeMMOff;
  int timeSSOff;
  
  bool soundAlarm;
  
  uint8_t reg_bit;
//  uint8_t pin;
  String pref;
};

RelayActuator ra[4] = {(1),(2),(3),(4)};;

void parameterReset() {
  for (uint8_t i=0; i<4; i++)
    _temps[i] = emptySignal;

  _atm_t = emptySignal;
  _atm_prs_mmHg = emptySignal;
  _bar_alt = emptySignal;
  
  for (uint8_t i=0; i<4; i++)
    _rOn[i] = ra[i].STATUS();
}

void parameterSet0() {
  for (uint8_t i=0; i<4; i++)
    _temps[i] = temps[i];
    
  for (uint8_t i=0; i<4; i++)
    _rOn[i] = ra[i].STATUS();
}

void parameterSet1() {
  _atm_t = roundFloat(atm_t, 1);
  _atm_prs_mmHg = roundFloat(atm_prs_mmHg, 1);
  _bar_alt = roundFloat(bar_alt, 1);
}

bool parameterCompare0() {
  for (uint8_t i=0; i<4; i++)
    if (_temps[i] != temps[i])
      return true;
    
  for (uint8_t i=0; i<4; i++)
    if (_rOn[i] != ra[i].STATUS())
      return true;
      
  return false;
}

bool parameterCompare1() {
  return _atm_t != roundFloat(atm_t, 1) || _atm_prs_mmHg != roundFloat(atm_prs_mmHg, 1) || _bar_alt != roundFloat(bar_alt, 1);
}

void draw(bool redraw = false) {
  if (redraw)
    disp.clear();
    
  if (page == 0) {
    if (redraw || parameterCompare0()) {
      disp.beginDraw();
      for (uint8_t i=0; i<4; i++) {
        String temp = fmtTemperature(getTempSensor(i), 1);
        disp.drawTemp(i, temp);
      }
      for (uint8_t i=0; i<4; i++) {
        disp.drawRelay(i, ra[i].STATUS());
      }
      disp.endDraw();
      parameterSet0();
    }
  } else if (page == 1) {
    if (redraw || parameterCompare1()) {
      disp.beginDraw();
      String AtmT = fmtTemperature(atm_t, 1);
      String AtmP = fmtPressure(atm_prs_mmHg, 1);
      String Alt = fmtAlt(bar_alt, 1);
      disp.drawPage2(AtmT, AtmP, Alt);
      disp.endDraw();
      parameterSet1();
    }
  }
}

void nextPage() {
  page = 1 - page;
  draw(true);
  doSound(100, 5);
}

void prevPage() {
  page = 1 - main::page;
  draw(true);
  doSound(100, 5);
}

void actuate() {
    for (uint8_t i=0; i<4; i++)
      ra[i].actuate();
}

void timerCheck() {
    for (uint8_t i=0; i<4; i++)
      ra[i].timerCheck();
}


void startAllSensors() {
  // Start the DS18B20 sensor
  sensors.begin();
  
  DeviceAddress deviceAddress;
  while (oneWire.search(deviceAddress)) {
    if (sensors.validAddress(deviceAddress) && sensors.validFamily(deviceAddress)) {
      sensors.setResolution(deviceAddress, DS18_RESOLUTION);
    }
  }
  // Start the BMP280 sensor
  bmpBegan = bmp.begin(BMP280_ADDRESS_ALT);
  if (bmpBegan) {
    bmp.setSampling(
      Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
      Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
      Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
      Adafruit_BMP280::FILTER_X16,      /* Filtering. */
      Adafruit_BMP280::STANDBY_MS_500   /* Standby time. */
    );
  }
}

void readAllSensors() {
  //DS18B20
  sensors.requestTemperatures(); 
  for (uint8_t i=0; i<4; i++)
    temps[i] = sensors.getTempCByIndex(i);

  //BMP280
  if (bmpBegan) {
    atm_t = bmp.readTemperature();
    double atm_prs_Pa = bmp.readPressure();
    atm_prs_mmHg = atm_prs_Pa * 0.00750062; // Pa to mmHg
    bar_alt = bmp.readAltitude(BMP_PRESSURE0);
  }
}

void setup() {
  parameterReset();
  startAllSensors();
  disp.begin();
}

void init() {
  disp.setBacklight(config::conf["backlightOn"]);
  setSound(BUZZER, config::conf["soundOn"]);

  normP = config::conf["normalPressure"];
  
  for (uint8_t i=0; i<4; i++) {
    clbTemps[i] = config::conf["calibrate/T" + String(i+1)];
    atmCorr[i] = config::conf[String(i+1) + "/atmCorr"];
  }
  
  for (uint8_t i=0; i<4; i++)
    ra[i].init();
}

void begin() {
  readAllSensors();
  actuate();
}

void loop() {
  if ((millis() - lastUpdateTimer >= updateIntervalTimer) || lastUpdateTimer == 0) {
    lastUpdateTimer = millis();
    timerCheck();
  }
  if ((millis() - lastUpdate >= updateInterval) || lastUpdate == 0) {
    lastUpdate = millis();
    readAllSensors();
    actuate();
  }
  if (alarm) {
    if ((millis() - lastAlarmTimer >= alarmIntervalTimer) || lastAlarmTimer == 0) {
      lastAlarmTimer = millis();
      alarmSignal();
    }
  }
}

void welcome(bool startMelody) {
  disp.welcome();
  if (startMelody)
    startMelodyPlay();
  else
    delay(3000);
}

void displStartWiFi() {
  disp.startWiFi();
}

} //namespace

#endif
