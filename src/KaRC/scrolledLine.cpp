#include "scrolledLine.h"

#define STARTT 7000
#define SCROLLT 500

ScrolledLine::ScrolledLine() {
  _txtWidth = 0;
  _txtX = 0;
  _startTicks = STARTT;
  _scrollTicks = SCROLLT;

}

void ScrolledLine::init(HX1230_FB_EX *lcd, byte row, const uint8_t* font, byte charwidth)
{
  _row = row;
  _font = font;
  _charWidth = charwidth;
  _lcd = lcd;
}

void ScrolledLine::setText(const char *txt) {
  strcpy(_txt, txt);
  _clearScrolls();
}

void ScrolledLine::ticks() {
  _lcd->setFont(_font);
  _txtWidth = _lcd->strWidth(_txt);
  if (_txtWidth > 96) {
    _lcd->printStr(-_txtX, _row, _txt);
    _lcd->printStr(-_txtX + _txtWidth + _charWidth, _row, "*");
    _lcd->printStr(-_txtX + _txtWidth + _charWidth * 3, _row, _txt);
  } else
    _lcd->printStr(0, _row, _txt);
  if (_checkDelay(_txtX == 0 ? _startTicks : _scrollTicks, _scrollDelay)) _scroll();
}

void ScrolledLine::_scroll() {
  if (_txtWidth > 96) {
    _txtX += _charWidth * 2;
    //_txtX+=_charWidth;
    if (_txtX >= _txtWidth + _charWidth * 3) _txtX = 0;
  }
}

void ScrolledLine::_clearScrolls() {
  _txtX = 0;
  _scrollDelay = millis();
}

boolean ScrolledLine::_checkDelay(int m, unsigned long &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}
