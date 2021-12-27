#include "dbg.h"
#include "HXScreen.h"

HXScreen screen;

#include "timers.h"
#include "parser.h"
#include "network.h"
#include "buttons.h"

void setup() {
  dbgbegin(115200);
  setupButtons();
  screen.init();
  screen.drawSplash("connecting");
  connectToCli();
}

void loop() {
  buttonTicks();
  handleCli();
}
