#ifndef buttons_h
#define buttons_h

#define BTN2 D1
#define BTN3 D2
#define BTN4 D4

#include <OneButton.h>

OneButton btnRight(BTN2, true, true);
OneButton btnCenter(BTN3, true, true);
OneButton btnLeft(BTN4, true, true);

void volSet(int stepv) {
  char buf[15];
  int level = screen.getVolLevel() + stepv;
  if (level < 0) level = 0;
  if (level > 100) level = 100;
  screen.setVolLevel(level);
  sprintf(buf, "cli.vol(\"%d\")", level * 254 / 100);
  client.println(buf);
}

/*

   btnLeft

*/
void onClick_btnLeft() { //prev
  dbgln("onClick_btnLeft");
  //resetBackLightTimer();
  switch (screen.getScreenMode()) {
    case SCREENPLAY: {
        if (client.connected() == 1) {
          //client.println("cli.prev");
          volSet(-5);
          screen.showVolume();
          startVolumeTimer();

        }
        break;
      }
    case SCREENLIST: {
        screen.prevItem();
        break;
      }
  }
}

void onLongPressStart_btnLeft() {
  screen.powerOff();
}

void onDoubleClick_btnLeft() {
  //pass
}

void onMultiClick_btnLeft() {
  //pass
}
/*

   btnCenter

*/
void onClick_btnCenter() { //play
  dbgln("onClick_btnCenter");
  switch (screen.getScreenMode()) {
    case SCREENPLAY: {
        if (client.connected() == 1) {
          if (playing) {
            client.println("cli.stop");
          } else {
            client.println("cli.start");
          }
        }
        break;
      }
    case SCREENLIST: {
        if (client.connected() == 1) {
          char command[20];
          sprintf(command, "cli.play(\"%d\")", screen.currentItem());
          client.println(command);
          screen.setScreen(SCREENPLAY);
        }
        break;
      }
  }
}

void onDoubleClick_btnCenter() {
  dbgln("onDoubleClick_btnCenter");
  screen.showDialog(WiFi.localIP().toString().c_str(), 5);
}

void onLongPressStart_btnCenter() {
  dbgln("onLongPressStart_btnCenter");
  screen.stepMode();
}

/*

   btnRight

*/
void onClick_btnRight() { //next
  dbgln("onClick_btnRight");
  switch (screen.getScreenMode()) {
    case SCREENPLAY: {
        if (client.connected() == 1) {
          //client.println("cli.next");
          volSet(5);
          screen.showVolume();
          startVolumeTimer();

        }
        break;
      }
    case SCREENLIST: {
        screen.nextItem();
        break;
      }
  }
}

void onLongPressStart_btnRight() {
  dbgln("onLongPressStart1");
  screen.showDialog(WiFi.localIP().toString().c_str(), 5);
}

void setupButtons() {
  btnRight.setClickTicks(300);
  btnRight.setPressTicks(1000);
  btnRight.setDebounceTicks(80);

  btnCenter.setClickTicks(300);
  btnCenter.setPressTicks(1000);
  btnCenter.setDebounceTicks(80);

  btnLeft.setClickTicks(300);
  btnLeft.setPressTicks(1000);
  btnLeft.setDebounceTicks(80);

  btnLeft.attachClick(onClick_btnLeft);
  btnLeft.attachLongPressStart(onLongPressStart_btnLeft);
  btnLeft.attachDoubleClick(onDoubleClick_btnLeft);
  btnLeft.attachMultiClick(onMultiClick_btnLeft);
  btnCenter.attachLongPressStart(onLongPressStart_btnCenter);
  btnCenter.attachClick(onClick_btnCenter);
  btnCenter.attachDoubleClick(onDoubleClick_btnCenter);

  btnRight.attachClick(onClick_btnRight);
  btnRight.attachLongPressStart(onLongPressStart_btnRight);
}

void blCheck() {
  bool st = digitalRead(BTN2);
  if (!st) resetBackLightTimer();
  st = digitalRead(BTN3);
  if (!st) resetBackLightTimer();
  st = digitalRead(BTN4);
  if (!st) resetBackLightTimer();
}

void buttonTicks() {
  btnLeft.tick();
  btnCenter.tick();
  btnRight.tick();
  blCheck();
}

#endif
