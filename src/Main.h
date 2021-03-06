#ifndef Main_h
#define Main_h

#include <arduino.h>

//DomoChip Informations
//------------Compile for 1M 64K SPIFFS------------
//------------/!\/!\/!\------Use Flash DOUT!!!------/!\/!\/!\------------
//Configuration Web Pages :
//http://IP/
//http://IP/config
//http://IP/fw
//RGBW Request Web Pages
//http://IP/setColor?color=RRGGBB
//http://IP/setColor?color=RRGGBBWW
//http://IP/getColor
//http://IP/getJSONColor

//include Application header file
#include "WirelessRGBW.h"

#define APPLICATION1_NAME "WRGBW"
#define APPLICATION1_DESC "DomoChip Wireless RGBW"
#define APPLICATION1_CLASS WebRGBW

#define VERSION_NUMBER "3.1.1"

#define DEFAULT_AP_SSID "WirelessRGBW"
#define DEFAULT_AP_PSK "PasswordRGBW"

//For ARILUX AL-LC01, Pins used are fixed
//GPIO 14 = Red Leds
//GPIO 5 = Green Leds
//GPIO 12 = Blue Leds

//For ARILUX AL-LC02, Pins used are fixed
//GPIO 5 = Red Leds
//GPIO 14 = Green Leds
//GPIO 12 = Blue Leds
//GPIO 13 = White Leds

//Enable developper mode (fwdev webpage and SPIFFS is used)
#define DEVELOPPER_MODE 0

//Choose Serial Speed
#define SERIAL_SPEED 115200

//Choose Pin used to boot in Rescue Mode
#define RESCUE_BTN_PIN 4

//Status LED
//#define STATUS_LED_SETUP pinMode(XX, OUTPUT);pinMode(XX, OUTPUT);
//#define STATUS_LED_OFF digitalWrite(XX, HIGH);digitalWrite(XX, HIGH);
//#define STATUS_LED_ERROR digitalWrite(XX, HIGH);digitalWrite(XX, HIGH);
//#define STATUS_LED_WARNING digitalWrite(XX, HIGH);digitalWrite(XX, HIGH);
//#define STATUS_LED_GOOD digitalWrite(XX, HIGH);digitalWrite(XX, HIGH);

//construct Version text
#if DEVELOPPER_MODE
#define VERSION VERSION_NUMBER "-DEV"
#else
#define VERSION VERSION_NUMBER
#endif

#endif


