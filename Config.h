#ifndef CONFIG_H
#define CONFIG_H

#include "Display.h"
#include "MenuNavigate.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

namespace config {

enum enTYPES {NONE, STRING, INT, FLOAT, ONOFF, ENWFM, RELM, SNSNO, CMPOPER};
enum enWIFIMODE {AP, STA};
enum enRELMODE {AUTO, rmON, rmOFF};
enum enSENSNO {NA, T1, T2, T3, T4};
enum enOPER {LESS, MORE};

uint8_t buzzerPin;
typedef void OnConfigExitFunc();
OnConfigExitFunc* onConfigExit;

typedef void ResetScale(String);
ResetScale* resetScale;

//*****************
//saved config data
const int capacity = JSON_OBJECT_SIZE(1000);
StaticJsonDocument<capacity> conf;

void initConfig() {
  conf["soundOn"] = true;
  conf["melodyOn"] = true;
  conf["backlightOn"] = true;
  conf["wifiMode"] = AP;
  conf["normalPressure"] = 760;
  conf["scaleOffset/1"] = 0;
  conf["scaleScale/1"] = 0;
  conf["scaleStandard/1"] = 0;
  conf["scaleDensity/1"] = 1000;
  
  conf["1/mode"] = rmOFF;
  conf["1/on/sensor"] = T1;
  conf["1/on/oper"] = LESS;
  conf["1/on/temp"] = 78.3;
  conf["1/off/sensor"] = T1;
  conf["1/off/oper"] = MORE;
  conf["1/off/temp"] = 78.4;
  conf["1/atmCorr"] = -0.03;
  conf["1/sound"] = false;
  
  conf["2/mode"] = rmOFF;
  conf["2/on/sensor"] = T2;
  conf["2/on/oper"] = LESS;
  conf["2/on/temp"] = 80.0;
  conf["2/off/sensor"] = T2;
  conf["2/off/oper"] = MORE;
  conf["2/off/temp"] = 81.0;
  conf["2/atmCorr"] = -0.03;
  conf["2/sound"] = false;
  
  conf["3/mode"] = rmOFF;
  conf["3/on/sensor"] = T3;
  conf["3/on/oper"] = LESS;
  conf["3/on/temp"] = 90.0;
  conf["3/off/sensor"] = T3;
  conf["3/off/oper"] = MORE;
  conf["3/off/temp"] = 91.0;
  conf["3/atmCorr"] = -0.03;
  conf["3/sound"] = false;
  
  conf["4/mode"] = rmOFF;
  conf["4/on/sensor"] = T4;
  conf["4/on/oper"] = LESS;
  conf["4/on/temp"] = 99.0;
  conf["4/off/sensor"] = T4;
  conf["4/off/oper"] = MORE;
  conf["4/off/temp"] = 100.0;
  conf["4/atmCorr"] = -0.03;
  conf["4/sound"] = false;

  conf["1/on/timeMM"] = 0;
  conf["1/on/timeSS"] = 0;
  conf["1/off/timeMM"] = 0;
  conf["1/off/timeSS"] = 0;
  conf["2/on/timeMM"] = 0;
  conf["2/on/timeSS"] = 0;
  conf["2/off/timeMM"] = 0;
  conf["2/off/timeSS"] = 0;
  conf["3/on/timeMM"] = 0;
  conf["3/on/timeSS"] = 0;
  conf["3/off/timeMM"] = 0;
  conf["3/off/timeSS"] = 0;
  conf["4/on/timeMM"] = 0;
  conf["4/on/timeSS"] = 0;
  conf["4/off/timeMM"] = 0;
  conf["4/off/timeSS"] = 0;
}

void saveConfig() {
  LittleFS.begin();
  File file = LittleFS.open("config.txt", "w");
  serializeJson(conf, file);
}

void loadConfig() {
  LittleFS.begin();
  File file = LittleFS.open("config.txt", "r");
  if (file)
    deserializeJson(conf, file);
}
//*****************

class ConfigItem : public MenuItemBase {
  public:
    ConfigItem(String title) : MenuItemBase(title) {};
    ConfigItem(String title, bool backItem) : MenuItemBase(title, backItem) {};
    ConfigItem(String title, bool backItem, bool titleDisplay) : MenuItemBase(title, backItem, titleDisplay) {};
    ConfigItem(String title, OnSelectFunk* onSelect) : MenuItemBase(title, onSelect) {};

    ConfigItem(String title, String dataPath) : MenuItemBase(title), dataPath(dataPath) {};
    ConfigItem(String title, String dataPath, enTYPES type, OnSelectFunk* onSelect) : MenuItemBase(title, onSelect), dataPath(dataPath), type(type) {};
    
    String dataPath;
    int intMin = 0;
    int intMax = 0;
    uint8_t floatPrec = 1;
    enTYPES type = NONE;
    
    String GetDataAsString() {
      switch (type) {
        case STRING: {
          String strVal = conf[dataPath];
          return strVal;
        }
        case INT: {
          String strVal = String((int)conf[dataPath]);
          return strVal;
        }
        case FLOAT: {
          float val = conf[dataPath];
          String strVal = String(val, floatPrec);
          
          float rVal = roundPrec(val, floatPrec);
          if (rVal > 0)
            strVal = "+" + strVal;
          else if (rVal == 0)
            strVal = " " + strVal;
          return strVal;
        }
        case ONOFF:
          return conf[dataPath] ? "ON" : "OFF";
        case ENWFM:
          return (enWIFIMODE)conf[dataPath] == AP ? "AP" : "STA";
        case RELM:
          switch ((enRELMODE)conf[dataPath]) {
            case AUTO:
              return "AUTO";
            case rmON:
              return "ON";
            case rmOFF:
              return "OFF";
            default:
              return "";
          }
        case SNSNO:
          switch ((enSENSNO)conf[dataPath]) {
            case T1:
              return "T1";
            case T2:
              return "T2";
            case T3:
              return "T3";
            case T4:
              return "T4";
            default:
              return "--";
          }
        case CMPOPER:
          return (enOPER)conf[dataPath] == MORE ? ">" : "<";
        default:
          return "";
      }
    }
};

#ifdef LCD
MenuDisplayPCF8574 disp;
#else
MenuDisplaySSD1306 disp;
#endif

ConfigItem menu("MAIN MENU");
MenuNavigate nav(&menu, &disp);

void SoundSelect(MenuItemBase* menuItem) {
  conf["soundOn"] = !conf["soundOn"];
  nav.setSound(buzzerPin, conf["soundOn"]);
  disp.DrawField(menuItem);
}

void MelodySelect(MenuItemBase* menuItem) {
  conf["melodyOn"] = !conf["melodyOn"];
  disp.DrawField(menuItem);
}

void BacklightSelect(MenuItemBase* menuItem) {
  conf["backlightOn"] = !conf["backlightOn"];
  disp.setBacklight(conf["backlightOn"]);
  disp.DrawField(menuItem);
}

void WiFiModeSelect(MenuItemBase* menuItem) {
  if ((enWIFIMODE)conf["wifiMode"] == AP)
    conf["wifiMode"] = STA;
  else
    conf["wifiMode"] = AP;
  disp.DrawField(menuItem);
}

void RelayModeSelect(MenuItemBase* menuItem) {
  ConfigItem* item = static_cast<ConfigItem*>(menuItem);
  switch ((enRELMODE)conf[item->dataPath]) {
    case AUTO:
      conf[item->dataPath] = rmON;
      break;
    case rmON:
      conf[item->dataPath] = rmOFF;
      break;
    default:
      conf[item->dataPath] = AUTO;
  }
  disp.DrawField(menuItem);
}

void SoundAlarmSelect(MenuItemBase* menuItem) {
  ConfigItem* item = static_cast<ConfigItem*>(menuItem);
  conf[item->dataPath] = !conf[item->dataPath];
  disp.DrawField(menuItem);
}

void SensorSelect(MenuItemBase* menuItem) {
  ConfigItem* item = static_cast<ConfigItem*>(menuItem);
  switch ((enSENSNO)conf[item->dataPath]) {
    case T1:
      conf[item->dataPath] = T2;
      break;
    case T2:
      conf[item->dataPath] = T3;
      break;
    case T3:
      conf[item->dataPath] = T4;
      break;
    case T4:
      conf[item->dataPath] = NA;
      break;
    default:
      conf[item->dataPath] = T1;
  }
  disp.DrawField(menuItem);
}

void OperSelect(MenuItemBase* menuItem) {
  ConfigItem* item = static_cast<ConfigItem*>(menuItem);
  switch ((enOPER)conf[item->dataPath]) {
    case LESS:
      conf[item->dataPath] = MORE;
      break;
    default:
      conf[item->dataPath] = LESS;
  }
  disp.DrawField(menuItem);
}

void CalibrateScaleSelect(MenuItemBase* menuItem) {
  ConfigItem* item = static_cast<ConfigItem*>(menuItem);
  resetScale(item->dataPath);
  nav.doSound(1000, 15);
}


bool adjustFieldMode = false;
void (*adjustField)(bool);
ConfigItem* adjustItem;

unsigned long lastAdjust = 0;

void adjustFloatField(bool increase) {
  bool fineAdjust = (millis() - lastAdjust) > 35;
  lastAdjust = millis();
  
  float adjStep;
  if (fineAdjust)
    adjStep = 1 / pow(10, adjustItem->floatPrec);
  else
    adjStep = 1;

  float val = conf[adjustItem->dataPath];
  val += (increase ? adjStep : -adjStep);
  conf[adjustItem->dataPath] = val;
  disp.DrawField(adjustItem, true);
}

void FloatDataSelect(MenuItemBase* menuItem) {
  adjustFieldMode = true;
  adjustField = adjustFloatField;
  adjustItem = static_cast<ConfigItem*>(menuItem);
  disp.DrawField(adjustItem, true);
}

void adjustIntField(bool increase) {
  bool fineAdjust = (millis() - lastAdjust) > 35;
  lastAdjust = millis();

  int adjStep;
  if (fineAdjust)
    adjStep = 1;
  else
    adjStep = 100;

  int val = conf[adjustItem->dataPath];
  val += (increase ? adjStep : -adjStep);

  if (adjustItem->intMax != adjustItem->intMin)
    if (val > adjustItem->intMax)
      val = adjustItem->intMax;
    else if (val < adjustItem->intMin)
      val = adjustItem->intMin;
      
  conf[adjustItem->dataPath] = val;
  disp.DrawField(adjustItem, true);
}

void IntDataSelect(MenuItemBase* menuItem) {
  adjustFieldMode = true;
  adjustField = adjustIntField;
  adjustItem = static_cast<ConfigItem*>(menuItem);
  disp.DrawField(adjustItem, true);
}

void adjustFieldEnd() {
  adjustFieldMode = false;
  disp.DrawField(adjustItem, false);
}

void DefaultSelect(MenuItemBase* menuItem) {
  initConfig();
}

void ResetWiFi(MenuItemBase* menuItem) {
  WiFiManager wifiManager;
  wifiManager.resetSettings();
}


void SaveExitSelect(MenuItemBase* menuItem) {
  saveConfig();
  if (onConfigExit)
    onConfigExit();
}

void ExitNoSaveSelect(MenuItemBase* menuItem) {
  loadConfig();
  if (onConfigExit)
    onConfigExit();
}


ConfigItem* newCalibrateItem(String title) {
  ConfigItem* item = new ConfigItem(title, "calibrate/" + title, FLOAT, FloatDataSelect); //num +/-NN.NN
  item->floatPrec = 2;
  return item;
}

ConfigItem* newCalibrateGroupItem(String title) {
  ConfigItem* item = new ConfigItem(title);
  item->subItems.push_back(newCalibrateItem("T1"));
  item->subItems.push_back(newCalibrateItem("T2"));
  item->subItems.push_back(newCalibrateItem("T3"));
  item->subItems.push_back(newCalibrateItem("T4"));
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  return item;
}

ConfigItem* newSettingItem(String title) {
  ConfigItem* item = new ConfigItem(title);
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  item->subItems.push_back(newCalibrateGroupItem("Calibrate"));
  item->subItems.push_back(new ConfigItem("Norm atm prs", "normalPressure", FLOAT, FloatDataSelect));
  item->subItems.push_back(new ConfigItem("Sound", "soundOn", ONOFF, SoundSelect));             //on/off
  item->subItems.push_back(new ConfigItem("Melody", "melodyOn", ONOFF, MelodySelect));          //on/off
  item->subItems.push_back(new ConfigItem("Backlight", "backlightOn", ONOFF, BacklightSelect)); //on/off
  item->subItems.push_back(new ConfigItem("WiFi mode", "wifiMode", ENWFM, WiFiModeSelect));     //enum AP/STA
  item->subItems.push_back(new ConfigItem("Reset WiFi", ResetWiFi));
  item->subItems.push_back(new ConfigItem("Reset to Default", DefaultSelect));
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  return item;
}

ConfigItem* newOnOffItem(String title, String path) {
  ConfigItem* item = new ConfigItem(title);
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  item->subItems.push_back(new ConfigItem("Sensor", path + "/sensor", SNSNO, SensorSelect));   //enum T1/T2/T3/T4
  item->subItems.push_back(new ConfigItem("Oper", path + "/oper", CMPOPER, OperSelect));       //enum >/<
  item->subItems.push_back(new ConfigItem("Temp", path + "/temp", FLOAT, FloatDataSelect));    //num -100 .. +150

  ConfigItem* subItem = new ConfigItem("Time MM", path + "/timeMM", INT, IntDataSelect);       //num 0 .. 240
  subItem->intMin = 0;
  subItem->intMax = 240;
  item->subItems.push_back(subItem);

  subItem = new ConfigItem("Time SS", path + "/timeSS", INT, IntDataSelect);       //num 0 .. 60
  subItem->intMin = 0;
  subItem->intMax = 60;
  item->subItems.push_back(subItem);
  
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  return item;
}

ConfigItem* newRelayItem(String title, String path) {
  ConfigItem* item = new ConfigItem(title);
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  item->subItems.push_back(new ConfigItem("Mode", path + "/mode", RELM, RelayModeSelect)); //enum Auto/On/Off
  item->subItems.push_back(newOnOffItem("ON rule", path + "/on"));
  item->subItems.push_back(newOnOffItem("OFF rule", path + "/off"));
  
  ConfigItem* subItem = new ConfigItem("Atm Correct", path + "/atmCorr", FLOAT, FloatDataSelect); //num +/-NN.NN
  subItem->floatPrec = 2;
  item->subItems.push_back(subItem);
  
  item->subItems.push_back(new ConfigItem("Sound Alarm", path + "/sound", ONOFF, SoundAlarmSelect)); //on/off
  
  item->subItems.push_back(new ConfigItem("<<BACK", true));
  return item;
}

ConfigItem* newScaleItem(String title, String path) {
  ConfigItem* item = new ConfigItem(title);
  item->subItems.push_back(new ConfigItem("<<BACK", true));

  ConfigItem* subItem = new ConfigItem("Density", "scaleDensity/" + path, INT, IntDataSelect);
  subItem->intMin = 0;
  subItem->intMax = 100000;
  item->subItems.push_back(subItem);
  
  subItem = new ConfigItem("Standard", "scaleStandard/" + path, INT, IntDataSelect);
  subItem->intMin = 0;
  subItem->intMax = 10000;
  item->subItems.push_back(subItem);
  
  item->subItems.push_back(new ConfigItem("Calibrate", path, NONE, CalibrateScaleSelect));
  
  return item;
}

void buildMenu(ConfigItem& menu) {
  menu.subItems.push_back(new ConfigItem("EXIT w/o save", ExitNoSaveSelect));
  menu.subItems.push_back(newRelayItem("RELAY 1", "1"));
  menu.subItems.push_back(newRelayItem("RELAY 2", "2"));
  menu.subItems.push_back(newRelayItem("RELAY 3", "3"));
  menu.subItems.push_back(newRelayItem("RELAY 4", "4"));
  menu.subItems.push_back(newScaleItem("SCALE", "1"));
  menu.subItems.push_back(newSettingItem("SETTINGS"));
  menu.subItems.push_back(new ConfigItem("SAVE and exit", SaveExitSelect));
  menu.subItems.push_back(new ConfigItem("EXIT w/o save", ExitNoSaveSelect));
}

void setup(uint8_t buzPin, OnConfigExitFunc* onCfgExit, ResetScale* rstScale) {
  buzzerPin = buzPin;
  onConfigExit = onCfgExit;
  resetScale = rstScale;

  initConfig();
  loadConfig();
  buildMenu(menu);
}

void begin() {
  disp.begin();
  disp.clear();
  disp.setBacklight(conf["backlightOn"]);
  nav.begin();
  nav.setSound(buzzerPin, conf["soundOn"]);
  adjustFieldMode = false;
}

void onEncLeft(ESPRotary& rotary) {
  if (adjustFieldMode)
    adjustField(false);
  else
    nav.up();
}
void onEncRight(ESPRotary& rotary) {
  if (adjustFieldMode)
    adjustField(true);
  else
    nav.down();
}
void buttonPressed(Button2& btn) {
  if (adjustFieldMode)
    adjustFieldEnd();
  else
    nav.select();
}

} //namespace

#endif
