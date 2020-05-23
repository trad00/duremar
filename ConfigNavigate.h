#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <LiquidCrystal_PCF8574.h>
#include <AceButton.h>
#include <ESPRotary.h>
#include<vector> 

using namespace ace_button;
using namespace std;

class MenuItem {
  MenuItem(){};
  MenuItem(String title) : title(title) {};
  MenuItem(String title, bool titleDisplay) : title(title), titleDisplay(titleDisplay) {};
  MenuItem(String title, bool titleDisplay, bool backItem) : title(title), titleDisplay(titleDisplay), backItem(backItem) {};
  String title;
  bool titleDisplay = true;
  bool backItem = false;
  MenuItem* backMenu;
  void (*onSelect)();
  vector<MenuItem*> subItems;
  friend class ConfigNavigate;
};


class ConfigNavigate* thisConfigNavigate;
class ConfigNavigate {
  
  public:
    ConfigNavigate(int PinA, int PinB, int PinBtn, LiquidCrystal_PCF8574& lcd, byte lcdRows)
    : rotary(PinA, PinB, 4), button(PinBtn), lcd(lcd), lcdRows(lcdRows) {
      thisConfigNavigate = this;
      buildMenu();
      button.setEventHandler(buttonHandleEvent);
      rotary.setRightRotationHandler(rotRight);
      rotary.setLeftRotationHandler(rotLeft);
    }
    
    void (*onConfigExit)();
    void setConfigExit(void (*f)()) {
      onConfigExit = f;
    }
    
    void begin() {
      isStarted = true;
      menuLvl = 0;
      curMenu = &menu;
      menuEnter();
    }
    void end() {
      isStarted = false;
      menuLvl = -1;
      curMenu = 0;
      lcd.clear();
      onConfigExit();
    }
    inline bool started() {
      return isStarted;
    }
    
    void loop() {
      button.check();
      rotary.loop();
    }
    
  private:
    
    static void buttonHandleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
      Serial.println("buttonHandleEvent");
      switch (eventType) {
        case AceButton::kEventPressed:
          thisConfigNavigate->menuSelect();
          break;
      }
    }
    static void rotRight(ESPRotary& rotary) {
      thisConfigNavigate->menuDn();
    }
    static void rotLeft(ESPRotary& rotary) {
      thisConfigNavigate->menuUp();
    }

  private:
    AceButton button;
    ESPRotary rotary;
    LiquidCrystal_PCF8574& lcd;
    byte lcdRows;
    bool isStarted = false;

//******* Menu *********
  private:
    int startLine;
    int frameLines;
    int curLine;
    int prevLine;
    int frameOffset;
    int menuLvl = -1;
    int menuPos;
    MenuItem menu;
    MenuItem* curMenu;
  private:
    static MenuItem* newRelayItem(String title);
    static MenuItem* newOnOffItem(String title);
    static MenuItem* newSettingItem(String title);
    void buildMenu();
    void menuDraw(bool redraw);
    void menuUp();
    void menuDn();
    void menuSelect();
    void menuEnter();
    
    static void saveExit() {
      exit();
    };
    static void exit() {
      thisConfigNavigate->end();
    };
};

MenuItem* ConfigNavigate::newSettingItem(String title) {
  MenuItem* item = new MenuItem(title, false);
  item->subItems.push_back(new MenuItem("<<BACK", false, true));
  item->subItems.push_back(new MenuItem("sound"));
  item->subItems.push_back(new MenuItem("backlight"));
  item->subItems.push_back(new MenuItem("wifi mode"));
  item->subItems.push_back(new MenuItem("<<BACK", false, true));
  return item;
}

MenuItem* ConfigNavigate::newOnOffItem(String title) {
  MenuItem* item = new MenuItem(title, false);
  item->subItems.push_back(new MenuItem("<<BACK", false, true));
  item->subItems.push_back(new MenuItem("sensor"));
  item->subItems.push_back(new MenuItem("oper"));
  item->subItems.push_back(new MenuItem("temp"));
  item->subItems.push_back(new MenuItem("time"));
  item->subItems.push_back(new MenuItem("<<BACK", false, true));
  return item;
}

MenuItem* ConfigNavigate::newRelayItem(String title) {
  MenuItem* item = new MenuItem(title, false);
  item->subItems.push_back(new MenuItem("<<BACK", false, true));
  item->subItems.push_back(new MenuItem("Mode"));
  item->subItems.push_back(newOnOffItem("ON rule"));
  item->subItems.push_back(newOnOffItem("OFF rule"));
  item->subItems.push_back(new MenuItem("Atm Press"));
  item->subItems.push_back(new MenuItem("<<BACK", false, true));
  return item;
}

void ConfigNavigate::buildMenu() {
  
  menu.title = "MAIN MENU";
  menu.titleDisplay = false;
  menu.subItems.push_back(new MenuItem("Start/Stop"));
  menu.subItems.push_back(newRelayItem("RELAY 1"));
  menu.subItems.push_back(newRelayItem("RELAY 2"));
  menu.subItems.push_back(newRelayItem("RELAY 3"));
  menu.subItems.push_back(newRelayItem("RELAY 4"));
  menu.subItems.push_back(newSettingItem("SETTINGS"));

  MenuItem *item = new MenuItem("SAVE and exit");
  item->onSelect = saveExit;
  menu.subItems.push_back(item);
  
  item = new MenuItem("EXIT w/o save");
  item->onSelect = exit;
  menu.subItems.push_back(item);
}

void ConfigNavigate::menuSelect() {
  Serial.println("menuSelect");
  MenuItem* subItem = curMenu->subItems[menuPos];
  if (subItem->subItems.size() > 0) {
    subItem->backMenu = curMenu;
    curMenu = subItem;
    menuLvl++;
    menuEnter();
  }
  else if (subItem->backItem){
    curMenu = curMenu->backMenu;
    menuLvl--;
    menuEnter();
  }
  else if (subItem->onSelect) {
    subItem->onSelect();
    //subItem->dataPath
  }
}

void ConfigNavigate::menuEnter() {
  Serial.println("menuEnter");
  if (curMenu->subItems.size() == 0)
    return;
  
  startLine = curMenu->titleDisplay ? 1 : 0;
  frameLines = lcdRows - startLine;
  if (curMenu->subItems.size() < frameLines)
    frameLines = curMenu->subItems.size();
  
  curLine = startLine;
  prevLine = startLine;
  frameOffset = 0;
  menuPos = frameOffset+curLine;

  menuDraw(true);
}

void ConfigNavigate::menuDraw(bool redraw) {
  if (redraw){
    lcd.clear();
    int line = startLine;
    for (int i = frameOffset;  i<frameOffset+frameLines; i++) {
      lcd.setCursor(1, line++);
      if (i<curMenu->subItems.size()) {
        //lcd.print(menu[i].as<String>());
        lcd.print(curMenu->subItems[i]->title);
      }
    }
  }
  lcd.setCursor(0, prevLine);
  lcd.print(" ");
  lcd.setCursor(0, curLine);
  lcd.print(">");
  prevLine = curLine;
}

void ConfigNavigate::menuUp() {
  if (curLine > startLine) {
    //cursor in frame move
    curLine--;
    menuDraw(false);
  } else if (frameOffset > 0) {
    //frame offset move
    frameOffset--;
    menuDraw(true);
  }
  menuPos = frameOffset+curLine;
}

void ConfigNavigate::menuDn() {
  if (curLine < startLine+frameLines-1) {
    //cursor in frame move
    curLine++;
    menuDraw(false);
  } else if (frameOffset < curMenu->subItems.size()-frameLines) {
    //frame offset move
    frameOffset++;
    menuDraw(true);
  }
  menuPos = frameOffset+curLine;
}
