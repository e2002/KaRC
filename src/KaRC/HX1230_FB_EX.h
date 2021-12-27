#ifndef _HX1230_FB_EX_H
#define _HX1230_FB_EX_H

#include "HX1230_FB.h"

class HX1230_FB_EX: public HX1230_FB {
  public:
    HX1230_FB_EX(uint8_t rst, uint8_t cs);
    virtual void init();
    virtual int printChar(int xpos, int ypos, unsigned char c);
    virtual int charWidth(uint8_t _ch, bool last=true);
    virtual int printStr(int xpos, int ypos, char *str);
    virtual int strWidth(char *str);
  private:
    void localize(byte *strn);
};

#endif;
