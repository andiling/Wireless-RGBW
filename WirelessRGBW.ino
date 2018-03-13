#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "WirelessRGBW.h"


//Return JSON of AppData1 content
String AppData1::GetJSON() {

  String gc;

  gc = gc + F("\"m\":") + model;

  return gc;
}

//Parse HTTP Request into an AppData1 structure
bool AppData1::SetFromParameters(AsyncWebServerRequest* request, AppData1 &tempAppData1) {

  if (request->hasParam(F("m"), true)) tempAppData1.model = request->getParam(F("m"), true)->value().toInt();

  return true;
}







//------------------------------------------
//return RGBW Color
void WebRGBW::SetColor(byte* colorCode) {

  for (byte i = 0; i < _numberOfLeds; i++) _leds[i]->set(colorCode[i]);
}
//------------------------------------------
//return RGBW Status
String WebRGBW::GetColor(bool json = false) {

  String color('#');

  for (byte i = 0; i < _numberOfLeds; i++)  {
    String hex(_leds[i]->get(), HEX);
    if (hex.length() == 1) hex = '0' + hex;
    color += hex;
  }

  if (json) color = String(F("{\"color\":\"")) + color + F("\"}");

  return color;
}

//------------------------------------------
//return WebRGBW Status in JSON
String WebRGBW::GetStatus() {

  return GetColor(true);
}


//------------------------------------------
//Function to initiate WebRGBW with Config
void WebRGBW::Init(AppData1 &appData1) {

  Serial.print(F("Start WebRGBW"));

  _appData1 = &appData1;


  analogWriteRange(255);

  switch (_appData1->model) {
    case 1:
      _leds[0] = new FadeLed(5);
      _leds[1] = new FadeLed(14);
      _leds[2] = new FadeLed(12);
      _leds[3] = new FadeLed(13);
      _numberOfLeds = 4;
      break;

    default:
      _leds[0] = new FadeLed(14);
      _leds[1] = new FadeLed(5);
      _leds[2] = new FadeLed(12);
      _numberOfLeds = 3;
      break;
  }

  FadeLed::setInterval(20);
  for (byte i = 0; i < _numberOfLeds; i++) {
    _leds[i]->setGammaTable(gamma8, 255);
    _leds[i]->setTime(1000, true);
    _leds[i]->begin(0);
  }

  Run();

  //BIG FADE to check LEDS
  for (byte i = 0; i < _numberOfLeds; i++) {
    _leds[i]->on();
    while (!_leds[i]->done()) {
      yield(); //let ESP live and no WDT
      delay(5);
      Run();
    }
    _leds[i]->off();
    while (!_leds[i]->done()) {
      yield(); //let ESP live and no WDT
      delay(5);
      Run();
    }
  }

  _initialized = true;

  Serial.println(F(" : OK"));
}

//------------------------------------------
void WebRGBW::InitWebServer(AsyncWebServer &server) {

  server.on("/gs1", HTTP_GET, [this](AsyncWebServerRequest * request) {
    request->send(200, F("text/json"), GetStatus());
  });


  server.on("/setColor", HTTP_GET, [this](AsyncWebServerRequest * request) {

    //check WebRGBW is initialized
    if (!_initialized) {
      request->send(400, F("text/html"), F("RGBW not Initialized"));
      return;
    }

    //try to find color code
    if (!request->hasParam(F("color"))) {
      request->send(400, F("text/html"), F("Missing color"));
      return;
    }

    //check length of colorcode
    if (request->getParam(F("color"))->value().length() != (_numberOfLeds * 2)) {
      request->send(400, F("text/html"), F("Incorrect color size"));
      return;
    }

    const char * colorCodeA = request->getParam(F("color"))->value().c_str();
    //check that color code is hexa
    for (byte i = 0; i < strlen(colorCodeA); i++) {
      if (!isHexadecimalDigit(colorCodeA[i])) {
        request->send(400, F("text/html"), F("Incorrect color code"));
        return;
      }
    }

    //convert text to byte
    byte colorCode[4];
    for (byte i = 0; i < _numberOfLeds; i++) {
      colorCode[i] = Utils::AsciiToHex(colorCodeA[i * 2]) * 0x10 + Utils::AsciiToHex(colorCodeA[i * 2 + 1]);
    }

    SetColor(colorCode);
    request->send(200);
  });

  server.on("/getColor", HTTP_GET, [this](AsyncWebServerRequest * request) {
    request->send(200, F("text/html"), GetColor());
  });

  server.on("/getJSONColor", HTTP_GET, [this](AsyncWebServerRequest * request) {
    request->send(200, F("text/html"), GetColor(true));
  });

  server.on("/setTime", HTTP_GET, [this](AsyncWebServerRequest * request) {

    //try to find color code
    if (!request->hasParam(F("time"))) {
      request->send(400, F("text/html"), F("Missing time parameter"));
      return;
    }

    int time = request->getParam(F("time"))->value().toInt();

    //check time value
    if (!time || time > 100) {
      request->send(400, F("text/html"), F("Incorrect time value"));
      return;
    }

    for (byte i = 0; i < _numberOfLeds; i++) _leds[i]->setTime(time * 100, true);
    request->send(200);
  });
}

//------------------------------------------
//Run for timer
void WebRGBW::Run() {

  FadeLed::update();
}
