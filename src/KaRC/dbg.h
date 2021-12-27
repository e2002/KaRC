#ifndef dbg_h
#define dbg_h

//#define DBG

void dbgbegin(int speed) {
#ifdef DBG
  Serial.begin(speed);
  while (!Serial) {
    ;
  }
  delay(200);
#endif
}

void dbg(const char *str) {
#ifdef DBG
  Serial.print(str);
#endif
}

void dbgln(const char *str) {
#ifdef DBG
  Serial.println(str);
#endif
}

#endif
