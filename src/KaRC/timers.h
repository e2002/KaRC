#ifndef timers_h
#define timers_h
#include <Ticker.h>

int BACKLIGHTDELAY = 60; //in seconds;
int DEEPSLEEPDELAY = 1800; //in seconds;
int VOLUMEDELAY = 3; //in seconds;

Ticker delayTicks;
unsigned long backLighCount = 0;
unsigned long deepSleepCount = 0;
unsigned long volumeCount = 0;

void delayTick() {
  if (backLighCount > 0) {
    backLighCount++;
  }
  if (backLighCount > BACKLIGHTDELAY) {
    backLighCount = 0;
    digitalWrite(LCD_BL, LOW);
  }
  if (deepSleepCount > 0) {
    deepSleepCount++;
  }
  if (deepSleepCount > DEEPSLEEPDELAY) {
    deepSleepCount = 0;
    screen.powerOff();
  }
  if (volumeCount > 0) {
    volumeCount++;
  }
  if (volumeCount > VOLUMEDELAY) {
    screen.hideVolume();
    volumeCount = 0;

  }
}

void resetBackLightTimer() {
  backLighCount = 1;
  digitalWrite(LCD_BL, HIGH);
  deepSleepCount = 1;
}

void startDeepSleepTimer() {
  deepSleepCount = 1;
}

void stopDeepSleepTimer() {
  deepSleepCount = 0;
}

void startVolumeTimer() {
  volumeCount = 1;
  screen.showVolume();
}

#endif
