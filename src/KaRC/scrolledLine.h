#ifndef scrolledline_h
#define scrolledline_h

#include <Arduino.h>
#include "HX1230_FB_EX.h"
#include "HX1230Fonts.h"


class ScrolledLine {
  public:
    ScrolledLine();
    void setText(const char *txt);
    void init(HX1230_FB_EX *lcd, byte row, const uint8_t* font, byte charwidth);
    void ticks();
  private:
    HX1230_FB_EX *_lcd;
    byte _row;
    int _textWidth;
    const uint8_t* _font;
    char _txt[140];
    int _txtWidth;
    byte _charWidth;
    int _txtX, _startTicks, _scrollTicks;
    unsigned long _scrollDelay;
    boolean _checkDelay(int m, unsigned long &tstamp);
    void _scroll();
    void _clearScrolls();
};
#endif
