#ifndef AMAIN_H
#define AMAIN_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_PCF8574.h>


namespace main {

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_BMP280 bmp;
LiquidCrystal_PCF8574 lcd(DSPL_ADDR);

uint8_t page = 0;
uint8_t pinSound;
bool soundOn;
bool bmpBegan;

unsigned long lastUpdate = 0;
unsigned long updateInterval = 5000;


float _t[4];
double _bmp_t;
double _bmp_prs_Pa;
double _bmp_prs_mmHg;
double _bmp_alt;

float t[4] = {emptySignal,emptySignal,emptySignal,emptySignal};
float clb_t[4] = {0,0,0,0};
double bmp_t = emptySignal;
double bmp_prs_Pa = emptySignal;
double bmp_prs_mmHg = emptySignal;
double bmp_alt = emptySignal;

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

void parameterReset() {
  _t[0] = emptySignal;
  _t[1] = emptySignal;
  _t[2] = emptySignal;
  _t[3] = emptySignal;
  _bmp_t = emptySignal;
//  _bmp_prs_Pa = emptySignal;
  _bmp_prs_mmHg = emptySignal;
  _bmp_alt = emptySignal;
}

void parameterSet0() {
  _t[0] = t[0];
  _t[1] = t[1];
  _t[2] = t[2];
  _t[3] = t[3];
}

void parameterSet1() {
  _bmp_t = roundFloat(bmp_t, 1);
//  _bmp_prs_Pa = roundFloat(bmp_prs_Pa, 1);
  _bmp_prs_mmHg = roundFloat(bmp_prs_mmHg, 1);
  _bmp_alt = roundFloat(bmp_alt, 1);
}

bool parameterCompare0() {
  return _t[0] != t[0] || _t[1] != t[1] || _t[2] != t[2] || _t[3] != t[3];
}

bool parameterCompare1() {
  return _bmp_t != roundFloat(bmp_t, 1) || _bmp_prs_mmHg != roundFloat(bmp_prs_mmHg, 1) || _bmp_alt != roundFloat(bmp_alt, 1);
}

void draw(bool redraw = false) {
  if (redraw)
    lcd.clear();
    
  if (page == 0) {
    if (redraw || parameterCompare0()) {
//      lcd.clear();
      
      lcd.setCursor(0, 0);
      lcd.print("T1 ");
      lcd.print(fmtTemperature(t[0], 2));
      
      lcd.setCursor(0, 1);
      lcd.print("T2 ");
      lcd.print(fmtTemperature(t[1], 2));
      
      lcd.setCursor(0, 2);
      lcd.print("T3 ");
      lcd.print(fmtTemperature(t[2], 2));
      
      lcd.setCursor(0, 3);
      lcd.print("T4 ");
      lcd.print(fmtTemperature(t[3], 2));
      
      parameterSet0();
    }
  } else if (page == 1) {
    if (redraw || parameterCompare1()) {
//      lcd.clear();
      
      lcd.setCursor(0, 0);
      lcd.print("atm T ");
      lcd.print(fmtTemperature(bmp_t, 1));
      
      lcd.setCursor(0, 1);
      lcd.print("atm P ");
      lcd.print(fmtPressure(bmp_prs_mmHg, 1));
      
      lcd.setCursor(0, 2);
      lcd.print("alt   ");
      lcd.print(fmtAlt(bmp_alt, 1));

      parameterSet1();
    }
  }
}

void backlight(uint8_t backlightOn) {
  lcd.setBacklight(backlightOn);
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
  t[0] = sensors.getTempCByIndex(0) + clb_t[0];
  t[1] = sensors.getTempCByIndex(1) + clb_t[1];
  t[2] = sensors.getTempCByIndex(2) + clb_t[2];
  t[3] = sensors.getTempCByIndex(3) + clb_t[3];

  //BMP280
  if (bmpBegan) {
    bmp_t = bmp.readTemperature();
    bmp_prs_Pa = bmp.readPressure();
    bmp_prs_mmHg = bmp_prs_Pa * 0.00750062; // Pa to mmHg
    bmp_alt = bmp.readAltitude(BMP_PRESSURE0);
  }
}

void setup() {
  parameterReset();
  startAllSensors();
}

void init() {
  lcd.setBacklight(config::conf["backlightOn"]);
  setSound(BUZZER, config::conf["soundOn"]);
  clb_t[0] = config::conf["calibrate/T1"];
  clb_t[1] = config::conf["calibrate/T2"];
  clb_t[2] = config::conf["calibrate/T3"];
  clb_t[3] = config::conf["calibrate/T4"];
}

void actuate() {
  
}

void loop() {
  if ((millis() - lastUpdate >= updateInterval) || lastUpdate == 0) {
    lastUpdate = millis();
    readAllSensors();
    actuate();
  }
}

} //namespace

#endif
