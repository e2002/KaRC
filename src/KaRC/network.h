#ifndef network_h
#define network_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define AUTOCONNECT_MENU_TITLE  "KaRadio Remote Control"
#define AUTOCONNECT_APID  "KaRemote"
#define AUTOCONNECT_HOMEURI     "/_ac"
#define AUTOCONNECT_APPASS     "12345687"
#include <AutoConnect.h>

#include "pages.h"
#include "config.h"

IPAddress telnetip;
byte port       = 23;
bool bootStatus = false;

WiFiClient client;
AutoConnect       portal;
AutoConnectConfig config(AUTOCONNECT_APID, AUTOCONNECT_APPASS);
AutoConnectCredential stations(256);
AutoConnectAux  page_cli_setup;
AutoConnectAux  page_cli_save;
AutoConnectAux  page_ssid_del;
bool firstinfo = false;
bool playlistloaded = false;
int connectErrors = 0;

void rootPage();
bool atDetect(IPAddress& softapIP);

String cliAdd(AutoConnectAux& aux, PageArgument& args);
String cliSave(AutoConnectAux& aux, PageArgument& args);
String ssidDel(AutoConnectAux& aux, PageArgument& args);

bool isIp(char *str);
bool loadCliIp();
void loadDelays();

void connectToCli() {
  client.setTimeout (500);
  config.autoReconnect = true;
  config.ota = AC_OTA_BUILTIN;
  config.boundaryOffset = 256;
  config.portalTimeout = 10000;
  portal.config(config);

  page_cli_setup.load(PAGE_CLI_SETUP);
  page_cli_save.load(PAGE_CLI_SAVE);
  page_ssid_del.load(PAGE_DEL_SSID);

  portal.join({ page_cli_setup, page_cli_save, page_ssid_del });

  portal.on("/cli_setup", cliAdd);
  portal.on("/cli_save", cliSave);
  portal.on("/delssid", ssidDel);

  portal.onDetect(atDetect);
  delayTicks.attach_ms(1000, delayTick);
  dbgln("connect * connect * connect * connect");
  eepromRead(0, eeprom_config);
  if (eeprom_config.config_set != 4256) eepromSetDefaults();

  loadDelays();

  if (portal.begin()) {
    delay(500);
    WebServerClass& server = portal.host();
    server.on("/", rootPage);
    bootStatus = loadCliIp();
    if (bootStatus) {
      dbg("connecting to: ");
      dbg(telnetip.toString().c_str());
      dbg(":");
      dbgln("23");
      screen.bootPush("Connecting to");
      int ap = screen.bootPush(telnetip.toString().c_str());
      screen.bootCat(ap, ":23");
      if (!client.connect(telnetip, port)) {
        dbgln("connection failed");
        screen.bootPush("connection failed");
        return;
      }
      dbgln("connection OK");
      screen.bootPush("connection OK");
      client.setTimeout (100);
      if (client.available()) {
        dbgln("CLI available...");
        screen.bootPush("CLI available...");
      }
      screen.setScreen(SCREENPLAY);
    } else {
      dbgln("bootStatus = false");
      bootStatus = false;
      screen.bootPush("KA not available");
      screen.bootPush("Check config on");
      screen.bootPush("http://");
      screen.bootPush(WiFi.localIP().toString().c_str());
      screen.bootPush("/cli_setup");
    }
  }
}

bool loadCliIp() {
  uint8_t ent = stations.entries();
  String ssid = WiFi.SSID();
  int entcnt = ent > 5 ? 5 : ent;

  dbg("Number of saved stations=");
  dbgln(String(ent).c_str());
  station_config_t station_config;
  for (int i = 0; i < entcnt; i++) {
    stations.load(i, &station_config);
    if (ssid == String((const char*)station_config.ssid)) {
      for (byte k = 0; k < 4; k++)
        telnetip[k] = eeprom_config.ipaddresess[i][k];
      if (telnetip.isSet()) {
        return true;
        break;
      }
    }
  }
  return false;
}

void loadDelays() {
  BACKLIGHTDELAY = eeprom_config.bldelay;
  DEEPSLEEPDELAY = eeprom_config.dsdelay;
}

String cliAdd(AutoConnectAux& aux, PageArgument& args) {
  uint8_t ent = stations.entries();
  station_config_t station_config;
  int entcnt = ent > 5 ? 5 : ent;
  int i = 0;
  for (i; i < entcnt; i++) {
    stations.load(i, &station_config);
    aux["cli" + String(i + 1)].as<AutoConnectInput>().label = String((const char*)station_config.ssid);
    IPAddress ip(eeprom_config.ipaddresess[i][0], eeprom_config.ipaddresess[i][1], eeprom_config.ipaddresess[i][2], eeprom_config.ipaddresess[i][3]);
    if (ip.isSet()) {
      aux["cli" + String(i + 1)].as<AutoConnectInput>().value = ip.toString();
    }
  }
  for (i; i < 5; i++) {
    aux["cli" + String(i + 1)].as<AutoConnectInput>().enable = false;
  }
  aux["bldelay"].as<AutoConnectInput>().value = String(BACKLIGHTDELAY);
  aux["dsdelay"].as<AutoConnectInput>().value = String(DEEPSLEEPDELAY);
  return String();
}

String cliSave(AutoConnectAux& aux, PageArgument& args) {
  bool ipChanged = false;
  for (int i = 0; i < 5; i++) {
    String cliName = "cli" + String(i + 1);
    if (args.hasArg(cliName)) {
      IPAddress ip;
      ip.fromString(args.arg(cliName));
      for (byte k = 0; k < 4; k++) {
        if (!ipChanged) ipChanged = eeprom_config.ipaddresess[i][k] != ip[k];
        eeprom_config.ipaddresess[i][k] = ip[k];
      }
    }
  }
  BACKLIGHTDELAY = args.arg("bldelay").toInt();
  if (BACKLIGHTDELAY < 10) BACKLIGHTDELAY = 60;
  eeprom_config.bldelay = BACKLIGHTDELAY;
  DEEPSLEEPDELAY = args.arg("dsdelay").toInt();
  if (DEEPSLEEPDELAY < 10) DEEPSLEEPDELAY = 1800;
  eeprom_config.dsdelay = DEEPSLEEPDELAY;
  eepromWrite(0, eeprom_config);
  resetBackLightTimer();

  aux["jsstay"].as<AutoConnectText>().enable = !ipChanged;
  aux["jsreset"].as<AutoConnectText>().enable = ipChanged;
  return String();
}

/*
   TODO
*/
void clearCli(uint8_t id) {
  if (id > 5) return;
  for (byte i = id; i < 5; i++) {
    for (byte k = 0; k < 4; k++) {
      if (i < 4) {
        eeprom_config.ipaddresess[i][k] = eeprom_config.ipaddresess[i + 1][k];
      } else {
        eeprom_config.ipaddresess[i][k] = 0;
      }
    }
  }
  eepromWrite(0, eeprom_config);
}

String ssidDel(AutoConnectAux& aux, PageArgument& args) {
  uint8_t ent = stations.entries();
  station_config_t station_config;
  String out = "";
  AutoConnectCredential  ac(256);
  if (args.hasArg("num")) {
    int8_t  e = args.arg("num").toInt();
    out += "<h3>Request deletion # " + String(e) + "</h3>";
    ac.load(e, &station_config);
    if (ac.del((char *)station_config.ssid)) {
      delay(200);
      out += "<h3>Delete for " + String((char *)station_config.ssid) + " completed. Redirecting..</h3>";
      clearCli(e);
    } else {
      out += "<h3>Delete for " + String((char *)station_config.ssid) + " failed. Redirecting..</h3>";
    }
    out += "<script type='text/javascript'>setTimeout(function(){location.href='/_ac/reset'} , 2000);</script>";
    aux["ssidlist"].as<AutoConnectText>().value = out;
    return String();
  }
  out += "<input id=\"num\" type=\"hidden\" name=\"num\" value=\"0\">";
  for (int i = 0; i < ent; i++) {
    stations.load(i, &station_config);
    out += "<button id=\"sb\" type=\"submit\" name=\"Credential\" value=\"" + String(i) + "\" onclick=\"document.getElementById('num').value=this.value; _sa('/delssid')\">" + String((const char*)station_config.ssid) + " [delete]</button><br>";
  }
  aux["ssidlist"].as<AutoConnectText>().value = out;
  return String();
}

void handleCli() {
  if (!bootStatus) {
    portal.handleClient();
    delay(100);
    return;
  }
  while (client.available()) {
    String datastr = client.readStringUntil('\n');
    parse(&screen, datastr.c_str());
    if ( datastr == "> " && !firstinfo) {
      client.println("cli.info");
      firstinfo = true;
      delay(200);
    }
    if ( datastr == "> " && !playlistloaded) {
      client.println("cli.list");
      playlistloaded = true;
      resetBackLightTimer();
      delay(200);
    }
    dbgln(String("Recieving: " + datastr).c_str());
  }
  screen.display();
  portal.handleClient();
  delay(100);

  if (client.connected() != 1 || WiFi.status() != WL_CONNECTED)
  {
    dbg("Reconnect to CLI ");
    screen.bootPush("Reconnect to KA");
    screen.bootPush(telnetip.toString().c_str());
    byte pg = screen.bootPush("*");

    firstinfo = false;
    client.setTimeout (100);
    while (!client.connect(telnetip, port)) {
      connectErrors++;
      if (connectErrors % 10 == 0) {
        dbg(".");
        screen.bootCat(pg, "*");
      }
      if (connectErrors > 100) {
        bootStatus = false;
        connectErrors = 0;
        screen.bootPush("Connection error");
        screen.bootPush("Check config on");
        screen.bootPush("http://");
        screen.bootPush(WiFi.localIP().toString().c_str());
        screen.bootPush("/cli_setup");
        return;
      }
    }
    connectErrors = 0;
    dbgln(".");
    client.setTimeout (100);
    dbgln("Reconnect OK.");
    screen.bootPush("Reconnect OK");
    screen.setScreen(SCREENPLAY);
  }
}

bool atDetect(IPAddress& softapIP) {
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  Serial.println(AUTOCONNECT_APID);
  Serial.println(AUTOCONNECT_APPASS);
  screen.bootPush("AP mode started");
  screen.bootPush("---------------------");
  int n = screen.bootPush("AP:  ");
  screen.bootCat(n, AUTOCONNECT_APID);
  screen.bootPush("AP password:");
  screen.bootPush(AUTOCONNECT_APPASS);
  screen.bootPush("---------------------");
  screen.bootPush("IP address:");
  screen.bootPush(softapIP.toString().c_str());
  return true;
}

void rootPage() {
  WebServerClass& server = portal.host();
  server.sendHeader("Location", String("http://") + server.client().localIP().toString() + String("/_ac"));
  server.send(302, "text/plain", "");
  server.client().flush();
  server.client().stop();
}
#endif
