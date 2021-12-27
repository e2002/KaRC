#include <HX1230_FB.h>
#include "HX1230_FB_EX.h"
#include <SPI.h>

#define fontbyte(x) pgm_read_byte(&cfont.font[x])
#define CS_IDLE     digitalWrite(csPin, HIGH)
#define CS_ACTIVE   digitalWrite(csPin, LOW)

const uint8_t initData[] PROGMEM = {
  HX1230_POWER_ON,
  HX1230_CONTRAST + 16,
  HX1230_INVERT_OFF,
  HX1230_DISPLAY_NORMAL,
  HX1230_DISPLAY_ON,
  HX1230_SCAN_START_LINE + 0,
  HX1230_PAGE_ADDR + 0,
  HX1230_COL_ADDR_H + 0,
  HX1230_COL_ADDR_L + 0
};

HX1230_FB_EX::HX1230_FB_EX(uint8_t rst, uint8_t cs): HX1230_FB(rst, cs) {}

void HX1230_FB_EX::init()
{
  scrWd = SCR_WD;
  scrHt = SCR_HT / 8;
  isNumberFun = &isNumber;
  cr = 0;
  cfont.font = NULL;
  dualChar = 0;

  pinMode(csPin, OUTPUT);
  if (rstPin < 255) {
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(50);
    digitalWrite(rstPin, LOW);
    delay(5);
    digitalWrite(rstPin, HIGH);
    delay(10);
  }

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  CS_ACTIVE;
  for (int i = 0; i < sizeof(initData); i++) sendCmd(pgm_read_byte(initData + i));
  CS_IDLE;
}

void HX1230_FB_EX::localize(byte *strn)
{
  int index = 0;
  while (strn[index])
  {
    if (strn[index] > 0xBF)
    {
      switch (strn[index]) {
        case 0xD0: {
            if (strn[index + 1] == 0x81) {
              strn[index] = 0xA8;
              break;
            }
            if (strn[index + 1] >= 0x90 && strn[index + 1] <= 0xBF) strn[index] = strn[index + 1];
            break;
          }
        case 0xD1: {
            if (strn[index + 1] == 0x91) {
              strn[index] = 0xB7;
              break;
            }
            if (strn[index + 1] >= 0x80 && strn[index + 1] <= 0x8F) strn[index] = strn[index + 1];
            break;
          }
      }
      int sind = index + 2;
      while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
    }
    index++;
  }
}

int HX1230_FB_EX::charWidth(uint8_t c, bool last)
{
  if (c < cfont.firstCh || c > cfont.lastCh)
    return c == ' ' ?  1 + cfont.xSize / 2 : 0;
  if (cfont.xSize > 0) return cfont.xSize;
  int ys8 = (cfont.ySize + 7) / 8;
  int idx = 4 + (c - cfont.firstCh) * (-cfont.xSize * ys8 + 1);
  int wd = pgm_read_byte(cfont.font + idx);
  int wdL = 0, wdR = spacing; // default spacing before and behind char
  if ((*isNumberFun)(c) && cfont.minDigitWd > 0) {
    if (cfont.minDigitWd > wd) {
      wdL = (cfont.minDigitWd - wd) / 2;
      wdR += (cfont.minDigitWd - wd - wdL);
    }
  } else if (cfont.minCharWd > wd) {
    wdL = (cfont.minCharWd - wd) / 2;
    wdR += (cfont.minCharWd - wd - wdL);
  }
  return last ? wd + wdL + wdR : wd + wdL + wdR - spacing; // last!=0 -> get rid of last empty columns
}

int HX1230_FB_EX::printChar(int xpos, int ypos, unsigned char c)
{
  if (xpos >= SCR_WD || ypos >= SCR_HT)  return 0;
  int fht8 = (cfont.ySize + 7) / 8, wd, fwd = cfont.xSize;
  if (fwd < 0)  fwd = -fwd;

  if (c < cfont.firstCh || c > cfont.lastCh)  return c == ' ' ?  1 + fwd / 2 : 0;

  int x, y8, b, cdata = (c - cfont.firstCh) * (fwd * fht8 + 1) + 4;
  byte d;
  wd = fontbyte(cdata++);
  int wdL = 0, wdR = spacing;
  if ((*isNumberFun)(c) && cfont.minDigitWd > 0) {
    if (cfont.minDigitWd > wd) {
      wdL  = (cfont.minDigitWd - wd) / 2;
      wdR += (cfont.minDigitWd - wd - wdL);
    }
  } else if (cfont.minCharWd > wd) {
    wdL  = (cfont.minCharWd - wd) / 2;
    wdR += (cfont.minCharWd - wd - wdL);
  }

  int scrl = 0;
  if (xpos < 0) {
    scrl = -xpos;
    if (scrl > wd) return wd + wdL + wdR;
    xpos = 0;
  }
  if (xpos > SCR_WD) return wd + wdL + wdR;
  if (xpos + wd + wdL + wdR > SCR_WD) wdR = max(SCR_WD - xpos - wdL - wd, 0);
  if (xpos + wd + wdL + wdR > SCR_WD) wd  = max(SCR_WD - xpos - wdL, 0);
  if (xpos + wd + wdL + wdR > SCR_WD) wdL = max(SCR_WD - xpos, 0);

  for (x = scrl; x < wd; x++) {
    //byte mask = 1 << ((xpos+x+wdL)&7);
    for (y8 = 0; y8 < fht8; y8++) {
      d = fontbyte(cdata + x * fht8 + y8);
      int lastbit = cfont.ySize - y8 * 8;
      if (lastbit > 8) lastbit = 8;
      for (b = 0; b < lastbit; b++) {
        if (d & 1) scr[((ypos + y8 * 8 + b) / 8)*scrWd + xpos - scrl + x + wdL] |= 1 << ((ypos + y8 * 8 + b) & 7); //drawPixel(xpos+x+wdL, ypos+y8*8+b, 1);
        d >>= 1;
      }
    }
  }
  return wd + wdR + wdL;
}

int HX1230_FB_EX::strWidth(char *str)
{
  int wd = 0;
  localize((byte*)str);
  while (*str) wd += charWidth(*str++);
  return wd;
}

int HX1230_FB_EX::printStr(int xpos, int ypos, char *str)
{
  unsigned char ch;
  int stl, row;
  int x = xpos;
  int y = ypos;
  int wd = strWidth(str);

  while (*str) {
    int wd = printChar(x, y, *str++);
    x += wd;
    if (cr && x >= SCR_WD) {
      x = 0;
      y += cfont.ySize;
      if (y > SCR_HT) y = 0;
    }
  }
  if (invertCh) fillRect(xpos, x - 1, y, y + cfont.ySize + 1, 2);
  return x;
}
