#ifndef config_h
#define config_h

#define STORED_IPS 5
#define EEPROM_SIZE 2048

struct config_t
{
  unsigned int config_set; //must be 4256
  unsigned int bldelay;
  unsigned int dsdelay;
  byte ipaddresess[STORED_IPS][4];
} eeprom_config;


template <class T> int eepromWrite(int ee, const T& value) {
    const byte* p = (const byte*)(const void*)&value;
    int i;
    EEPROM.begin(EEPROM_SIZE);
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    EEPROM.commit();
    delay(20);
    EEPROM.end();
    return i;
}

template <class T> int eepromRead(int ee, T& value) {
    byte* p = (byte*)(void*)&value;
    int i;
    EEPROM.begin(EEPROM_SIZE);
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    EEPROM.end();
    return i;
}

void eepromSetDefaults() {
  eeprom_config.config_set = 4256;
  eeprom_config.bldelay = 21;
  eeprom_config.dsdelay = 1801;
  for (byte i = 0; i < STORED_IPS; i++) {
    for (byte k = 0; k < 4; k++) {
      eeprom_config.ipaddresess[i][k] = 0;
    }
  }
}

#endif
