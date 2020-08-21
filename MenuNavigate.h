#ifndef MENU_NAVIGATE_H
#define MENU_NAVIGATE_H

#include <vector> 

class MenuItemBase;

class IMenuDisplay {
  public:
    virtual uint8_t getRowCount() = 0;
    virtual void beginDraw() = 0;
    virtual void endDraw() = 0;
    virtual void DrawTitle(MenuItemBase* item) = 0;
    virtual void DrawItem(MenuItemBase* item) = 0;
    virtual void selectLine(uint8_t line) = 0;
};

typedef void OnSelectFunk(MenuItemBase* menuItem);

class MenuItemBase {
  public:
    MenuItemBase(String title) : title(title){};
    MenuItemBase(String title, bool backItem) : title(title), backItem(backItem){};
    MenuItemBase(String title, bool backItem, bool titleDisplay) : title(title), backItem(backItem), titleDisplay(titleDisplay){};
    MenuItemBase(String title, OnSelectFunk* onSelect) : title(title), onSelect(onSelect){};
    virtual String GetTitle() {return title;}
//    virtual String FormatItem(uint8_t dataCol) {return title;}
    virtual String GetDataAsString() {return "";}
    
    String title;
    bool titleDisplay = false;
    bool backItem = false;
    uint8_t displayLine = 0;
    std::vector<MenuItemBase*> subItems;
    OnSelectFunk* onSelect;
  friend class MenuNavigate;
};

class MenuNavigate {
  public:
    MenuNavigate(MenuItemBase* menu, IMenuDisplay* display)
    : rootMenu(menu), display(display), pinSound(pinSound) {
      current.menu = menu;
      current.frameLines = display->getRowCount();
    }
    void begin();
    void end();
    void up();
    void down();
    void select();
    void setSound(uint8_t pin,bool on);
    
  private:
    void menuDraw(bool redraw);
    void menuEnter(MenuItemBase* menu);
    void menuExit();
    void updateMenuPos();
    inline void doSound(uint16_t frequency, uint16_t duration);
    inline void moveSound()  { doSound(100,  5); };
    inline void enterSound() { doSound(1000,15); };
    inline void exitSound()  { doSound(800, 15); };
    inline void selectSound(){ doSound(1000,15); };
    
  private:
    IMenuDisplay* display;
    MenuItemBase* rootMenu;
    uint8_t pinSound;
    bool soundOn = false;
    
    struct CurrentMenuData {
      MenuItemBase* menu;
      uint8_t startLine = 0;
      uint8_t frameLines;
      uint8_t line = 0;
      uint8_t frameOffset = 0;
      uint8_t menuPos;
    } current;
    
    std::vector<CurrentMenuData> breadcrumbs;
};

void MenuNavigate::begin() {
  breadcrumbs.clear();
  menuEnter(rootMenu);
}

void MenuNavigate::setSound(uint8_t pin, bool on) {
  pinSound = pin;
  soundOn = pinSound >= 0 && on;
};

void MenuNavigate::doSound(uint16_t frequency, uint16_t duration) {
  if (soundOn)
    tone(pinSound, frequency, duration);
}

void MenuNavigate::menuDraw(bool redraw) {
  if (redraw){
    display->beginDraw();
    if (current.menu->titleDisplay) {
      display->DrawTitle(current.menu);
    }
    for (uint8_t i = current.frameOffset;  i < current.frameOffset + current.frameLines; i++) {
      if (i < current.menu->subItems.size()) {
        display->DrawItem(current.menu->subItems[i]);
      }
    }
    display->endDraw();
  }
  display->selectLine(current.line);
}

void MenuNavigate::menuEnter(MenuItemBase* menu) {
  if (menu->subItems.size() == 0)
    return;
  
  breadcrumbs.push_back(current);
  
  current.menu = menu;
  current.startLine = current.menu->titleDisplay ? 1 : 0;
  current.frameLines = display->getRowCount() - current.startLine;
  if (current.menu->subItems.size() < current.frameLines)
    current.frameLines = current.menu->subItems.size();
  
  current.line = current.startLine;
  current.frameOffset = 0;
  updateMenuPos();
  menuDraw(true);
}

void MenuNavigate::menuExit() {
  current = breadcrumbs.back();
  breadcrumbs.pop_back();
  menuDraw(true);
}

void MenuNavigate::updateMenuPos() {
  current.menuPos = current.frameOffset + current.line;
  if (current.menu->titleDisplay)
    current.menuPos--;
}

void MenuNavigate::up() {
  if (current.line > current.startLine) {
    //cursor in frame move
    moveSound();
    current.line--;
    updateMenuPos();
    menuDraw(false);
  } else if (current.frameOffset > 0) {
    //frame offset move
    moveSound();
    current.frameOffset--;
    updateMenuPos();
    menuDraw(true);
  }
}

void MenuNavigate::down() {
  if (current.line < current.startLine + current.frameLines - 1) {
    //cursor in frame move
    moveSound();
    current.line++;
    updateMenuPos();
    menuDraw(false);
  } else if (current.frameOffset < current.menu->subItems.size() - current.frameLines) {
    //frame offset move
    moveSound();
    current.frameOffset++;
    updateMenuPos();
    menuDraw(true);
  }
}

void MenuNavigate::select() {
  MenuItemBase* subItem = current.menu->subItems[current.menuPos];
  subItem->displayLine = current.line;
  if (subItem->subItems.size() > 0) {
    enterSound();
    menuEnter(subItem);
  }
  else if (subItem->backItem){
    exitSound();
    menuExit();
  }
  else if (subItem->onSelect) {
    selectSound();
    subItem->onSelect(subItem);
  }
}

#endif
