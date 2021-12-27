#include "HXScreen.h"
#include "HX1230_FB_EX.h"
#include "term9x14_font.h"
#include "small4x6_font.h"

HX1230_FB_EX lcd(LCD_RST, LCD_CS);

struct plItem
{
  byte idx;
  char name[40];
};
plItem playList[255];


HXScreen::HXScreen() {
  screenMode = SCREENBOOT;
  playlistSize = 0;
  timestamp = 0;
}

void HXScreen::init() {
  lcd.init();
  lcd.cls();
  lcd.display();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  pinMode(VINPUT, INPUT);
  lcd.setFont(Small4x6PL);
  slNameset.init(&lcd, 0, HX1230RUS, 7);
  slTitle.init(&lcd, 20, MedvedFont1230, 5);
  slSong.init(&lcd, 29, MedvedFont1230, 5);
  timeTicks.attach_ms(1000, [this]() {
    this->clockTick();
  });
  battTick();
  battTicks.attach_ms(20000, [this]() {
    this->battTick();
  });
}

void HXScreen::setName(const char* txt) {
  slNameset.setText(txt);
}

void HXScreen::setTitle(const char* txt) {
  slTitle.setText(txt);
}

void HXScreen::setSong(const char* txt) {
  slSong.setText(txt);
}

void HXScreen::setTime(time_t ts) {
  isTimeSet = true;
  timestamp = ts;
}

void HXScreen::setVolLevel(int vol) {
  volInt = vol;
  sprintf(volLevel, "VOL: %d%%", vol);
}

int HXScreen::getVolLevel() {
  return volInt;
}

void HXScreen::drawClock() {
  if (isTimeSet) {
    struct tm *dt;
    char strdate[40];
    char strclock[40];
    int dM, dH;
    dt = gmtime(&timestamp);
    dH = dt->tm_hour - dt->tm_hour % 10 == 10 ? 4 : 0;
    dM = dt->tm_min - dt->tm_min % 10 == 10 ? 4 : 0;
    sprintf(strdate, "%02d.%02d.%04d", dt->tm_mday, (dt->tm_mon) + 1, dt->tm_year + 1900);
    sprintf(strclock, "%02d:%02d:%02d", dt->tm_hour, dt->tm_min, dt->tm_sec);
    int clcw = lcd.strWidth(strclock);
    lcd.setFont(MedvedFont1230);
    lcd.printStr(0, 61, strdate);
    lcd.printStr(97 - clcw, 61, strclock);
  }
}

void HXScreen::setScreen(byte sm) {
  if (screenMode == 0) bootClear();
  screenMode = sm;
}

void HXScreen::drawBoot() {
  lcd.cls();
  for (byte i = 0; i < BOOTSIZE; i++) {
    if (strlen(bootArray[i]) == 0) {
      lcd.display();
      return;
      break;
    }
    lcd.printStr(0, i * 8, bootArray[i]);
  }
  lcd.display();

}

void HXScreen::drawPlayList() {
  lcd.cls();
  lcd.setFont(MedvedFont1230);
  int row = 2;
  int delta = startNamesetNum - 4;
  if (delta < 0) delta = 0;
  if (delta > playlistSize - PLSIZE) delta = playlistSize - PLSIZE;
  for (byte i = 0; i < PLSIZE; i++) {
    if (strlen(playList[i + delta].name) == 0) {
      lcd.display();
      return;
      break;
    }
    lcd.printStr(2, row, playList[i + delta].name);
    if (i + delta == startNamesetNum) lcd.fillRect(0, row - 2, 96, 11, 2);
    row += 11;
  }
  lcd.display();
}

void HXScreen::showDialog(const char* s, unsigned int tout) {
  strcpy(dialog, s);
  dialogTimeout = tout;
  isDrawDialog = true;
}

void HXScreen::drawDialog() {
  if (isDrawDialog) {
    lcd.setFont(HX1230RUS);
    int w = lcd.strWidth(dialog);
    lcd.fillRect(0, 18, 96, 39, 0);
    lcd.drawRect(0, 18, 96, 39, 1);
    lcd.printStr((96 - w) / 2, 32, dialog);
  }
}

void HXScreen::drawVolumeDialog() {
  if (isDrawVolume) {
    lcd.setFont(HX1230RUS);
    char v[12];
    sprintf(v, "VOL %d%%", volInt);
    int w = lcd.strWidth(v);
    lcd.fillRect(10, 19, 76, 38, 0);
    lcd.drawRect(10, 19, 76, 38, 1);
    lcd.printStr((96 - w) / 2, 32, v);
  }
}

void HXScreen::drawPlayer() {
  slNameset.ticks();
  lcd.drawLineH(0, 96, 16, 1);
  slTitle.ticks();
  slSong.ticks();
  lcd.drawLineH(0, 96, 39, 1);
  lcd.printStr(0, 41, volLevel);
  lcd.printStr(0, 50, battLevel);
  lcd.drawLineH(0, 96, 58, 1);
  drawClock();
  drawVolumeDialog();
  drawDialog();
}

byte HXScreen::bootPush(const char* s) {
  for (byte i = 0; i < BOOTSIZE; i++) {
    if (strlen(bootArray[i]) == 0) {
      strcpy(bootArray[i], s);
      drawBoot();
      return i;
      break;
    }
  }
  for (byte i = 0; i < BOOTSIZE - 1; i++) {
    strcpy(bootArray[i], bootArray[i + 1]);
  }
  strcpy(bootArray[BOOTSIZE - 1], s);
  drawBoot();
  return BOOTSIZE - 1;
}

void HXScreen::bootCat(byte pos, const char* s) {
  strcat(bootArray[pos], s);
  drawBoot();
}

void HXScreen::plPush(byte ind, char* s) {
  strncpy(playList[playlistSize].name, s, 40);
  playList[playlistSize].idx = ind;
}

void HXScreen::drawSplash( char* s) {
  lcd.cls();
  //lcd.drawBitmap(logo, -2, 8 , 84, 48);
  lcd.drawBitmap(logo2bw, -2, 0 , 60, 60);
  lcd.setFont(MedvedFont1230);
  int strw = lcd.strWidth(s);
  lcd.printStr(48 - strw / 2, 58, s);
  lcd.display();
}

void HXScreen::stepMode() {
  if (screenMode == SCREENBOOT) return;
  screenMode++;
  if (screenMode == SCREENLIST) {
    for (int i = 0; i < playlistSize; i++) {
      if (playList[i].idx == nsNum) {
        startNamesetNum = i;
        break;
      }
    }
  }
  if (screenMode > SCREENLIST) screenMode = SCREENPLAY;
}

void HXScreen::nextItem() {
  startNamesetNum++;
  if (startNamesetNum > playlistSize - 1) startNamesetNum = playlistSize - 1;
}

void HXScreen::prevItem() {
  startNamesetNum--;
  if (startNamesetNum < 0) startNamesetNum = 0;
}

int HXScreen::currentItem() {
  return playList[startNamesetNum].idx;
}

void HXScreen::playlistSizePlus() {
  playlistSize++;
}

byte HXScreen::getScreenMode() {
  return screenMode;
}

void HXScreen::powerOff() {
  digitalWrite(LCD_BL, LOW);
  lcd.displayMode(HX1230_POWER_OFF);
  ESP.deepSleep(0);
}

void HXScreen::showVolume() {
  isDrawVolume = true;
}

void HXScreen::hideVolume() {
  isDrawVolume = false;
}

void HXScreen::bootClear() {
  for (byte i = 0; i < BOOTSIZE; i++) {
    strcpy(bootArray[i], "");
  }
}

void HXScreen::clockTick() {
  timestamp++;
  if (dialogTimeout > 0) {
    dialogTimeout--;
  } else {
    isDrawDialog = false;
  }
}

void HXScreen::battTick() {
  long sum = 0;
  float voltage = 0.0;
  const float battery_max = 4.04;
  const float battery_min = 2.95;
  char vfloatStr[10];
  float voltageValue;
  int voltagePercent;
  for (int i = 0; i < 20; i++)
  {
    sum += analogRead(VINPUT);
    delayMicroseconds(1000);
  }

  voltage = sum / (float)20;
  voltageValue = voltage / 178;
  voltagePercent = map(voltageValue * 100, 295, 404, 0, 100);
  if (voltagePercent > 100)
    voltagePercent = 100;

  dtostrf(voltageValue, 4, 2, vfloatStr);
  sprintf(battLevel, "BAT: %d%% %sV", voltagePercent, vfloatStr);
}

void HXScreen::display() {
  lcd.cls();
  switch (screenMode) {
    case SCREENBOOT: {
        //drawBoot();
        break;
      }
    case SCREENPLAY: {
        drawPlayer();
        break;
      }
    case SCREENLIST: {
        drawPlayList();
        break;
      }
  }
  lcd.display();
}
