#ifndef hxscreen_h
#define hxscreen_h

#include <Arduino.h>
#include <Ticker.h>
#include "scrolledLine.h"

#define LCD_RST D3
#define LCD_CS  D8
#define LCD_BL  D6
#define LCD_DIN D7
#define LCD_CLK D5
#define VINPUT A0

#define SCREENBOOT 0
#define SCREENPLAY 1
#define SCREENLIST 2

#define BOOTSIZE 8
#define PLSIZE   6

class HXScreen {
  public:
    HXScreen();
    void init();
    void display();
    void setName(const char* txt);
    void setTitle(const char* txt);
    void setSong(const char* txt);
    void setTime(time_t ts);
    void setVolLevel(int vol);
    int getVolLevel();
    void setScreen(byte sm);
    byte bootPush(const char* s);
    void bootCat(byte pos, const char* s);
    void plPush(byte ind, char* s);
    void drawSplash( char* s);
    void stepMode();
    void prevItem();
    void nextItem();
    int currentItem();
    void playlistSizePlus();
    byte getScreenMode();
    void powerOff();
    void hideVolume();
    void showVolume();
    void showDialog(const char* s, unsigned int tout);
    time_t timestamp;
    byte nsNum;
    
  private:
    ScrolledLine slNameset;
    ScrolledLine slTitle;
    ScrolledLine slSong;
    Ticker timeTicks;
    Ticker battTicks;
    byte screenMode;
    int startNamesetNum;
    int playlistSize;
    bool isTimeSet = false;
    bool isDrawVolume = false;
    bool isDrawDialog = false;
    char dialog[30];
    unsigned int dialogTimeout=0;
    char bootArray[BOOTSIZE][40];
    char volLevel[10];
    int volInt;
    char battLevel[40];
    void drawClock();
    void drawBoot();
    void drawPlayer();
    void drawPlayList();
    void drawVolumeDialog();
    void drawDialog();
    void bootClear();
    void clockTick();
    void battTick();
};

#endif
