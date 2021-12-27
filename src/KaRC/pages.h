#ifndef pages_h
#define pages_h

const char PAGE_CLI_SETUP[] PROGMEM = R"(
{
  "uri": "/cli_setup",
  "title": "KaRadio",
  "menu": true,
  "element": [
    {
      "name": "cli1",
      "type": "ACInput",
      "label": "-",
      "apply": "text",
      "placeholder": "0.0.0.0"
    },
    {
      "name": "cli2",
      "type": "ACInput",
      "label": "-",
      "apply": "text",
      "placeholder": "0.0.0.0"
    },
    {
      "name": "cli3",
      "type": "ACInput",
      "label": "-",
      "apply": "text",
      "placeholder": "0.0.0.0"
    },
    {
      "name": "cli4",
      "type": "ACInput",
      "label": "-",
      "apply": "text",
      "placeholder": "0.0.0.0"
    },
    {
      "name": "cli5",
      "type": "ACInput",
      "label": "-",
      "apply": "text",
      "placeholder": "0.0.0.0"
    },
    {
      "name": "bldelay",
      "type": "ACInput",
      "label": "BackLight delay",
      "apply": "number",
      "placeholder": ""
    },
    {
      "name": "dsdelay",
      "type": "ACInput",
      "label": "DeepSleep delay",
      "apply": "number",
      "placeholder": ""
    },    
    {
      "name": "add",
      "type": "ACSubmit",
      "value": "APPLY",
      "uri": "/cli_save"
    }
  ]
}
)";

const char PAGE_CLI_SAVE[] PROGMEM = R"(
{
  "uri": "/cli_save",
  "title": "KaRadio",
  "menu": false,
  "element": [
    {
      "name": "jsstay",
      "type": "ACElement",
      "value": "<h2>Saved</h2><script type='text/javascript'>setTimeout(function(){location.href='/cli_setup'} , 2000);</script>"
    },
    {
      "name": "jsreset",
      "type": "ACElement",
      "value": "<h2>Saved, reseting</h2><script type='text/javascript'>setTimeout(function(){location.href='/_ac/reset'} , 2000);</script>"
    }
  ]
}
)";

const char PAGE_DEL_SSID[] PROGMEM = R"(
{
  "uri": "/delssid",
  "title": "Delete SSID",
  "menu": true,
  "element": [
    {
      "name": "ssidlist",
      "type": "ACElement",
      "value": "<h1>123</h1>"
    }
  ]
}
)";

#endif
