#include "WirelessRGBW.h"

//------------------------------------------
//return RGBW Color
void WebRGBW::SetColor(byte *colorCode)
{

  for (byte i = 0; i < _numberOfLeds; i++)
    _leds[i]->set(colorCode[i]);
}
//------------------------------------------
//return RGBW Status
String WebRGBW::GetColor(bool json = false)
{

  String color('#');

  for (byte i = 0; i < _numberOfLeds; i++)
  {
    String hex(_leds[i]->get(), HEX);
    if (hex.length() == 1)
      hex = '0' + hex;
    color += hex;
  }

  if (json)
    color = String(F("{\"color\":\"")) + color + F("\"}");

  return color;
}

//------------------------------------------
//Used to initialize configuration properties to default values
void WebRGBW::SetConfigDefaultValues()
{
  model = 0; //AL-LC01
};
//------------------------------------------
//Parse JSON object into configuration properties
void WebRGBW::ParseConfigJSON(JsonObject &root)
{
  if (root["m"].success())
    model = root[F("m")];
};
//------------------------------------------
//Parse HTTP POST parameters in request into configuration properties
bool WebRGBW::ParseConfigWebRequest(AsyncWebServerRequest *request)
{
  if (request->hasParam(F("m"), true))
    model = request->getParam(F("m"), true)->value().toInt();

  return true;
};
//------------------------------------------
//Generate JSON from configuration properties
String WebRGBW::GenerateConfigJSON(bool forSaveFile = false)
{
  String gc('{');

  gc = gc + F("\"m\":") + model;

  gc += '}';

  return gc;
};
//------------------------------------------
//Generate JSON of application status
String WebRGBW::GenerateStatusJSON()
{
  return GetColor(true);
};
//------------------------------------------
//code to execute during initialization and reinitialization of the app
bool WebRGBW::AppInit(bool reInit)
{
  if (reInit)
  {
    for (byte i = 0; i < _numberOfLeds; i++)
      if (_leds[i] != nullptr)
      {
        _leds[i]->~FadeLed();
        _leds[i] = nullptr;
      }
  }

  analogWriteRange(255);
  switch (model)
  {
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
    _leds[3] = NULL;
    _numberOfLeds = 3;
    break;
  }

  FadeLed::setInterval(20);
  for (byte i = 0; i < _numberOfLeds; i++)
  {
    _leds[i]->setGammaTable(gamma8, 255);
    _leds[i]->setTime(1000, true);
    _leds[i]->begin(0);
  }

  FadeLed::update();

  //BIG FADE to check LEDS
  for (byte i = 0; i < _numberOfLeds; i++)
  {
    _leds[i]->on();
    while (!_leds[i]->done())
    {
      yield(); //let ESP live and no WDT
      delay(5);
      FadeLed::update();
    }
    _leds[i]->off();
    while (!_leds[i]->done())
    {
      yield(); //let ESP live and no WDT
      delay(5);
      FadeLed::update();
    }
  }

  return true;
};
//------------------------------------------
//code to register web request answer to the web server
void WebRGBW::AppInitWebServer(AsyncWebServer &server, bool &shouldReboot, bool &pauseApplication)
{
  server.on("/setColor", HTTP_GET, [this](AsyncWebServerRequest *request) {

    //try to find color code
    if (!request->hasParam(F("color")))
    {
      request->send(400, F("text/html"), F("Missing color"));
      return;
    }

    //check length of colorcode
    if (request->getParam(F("color"))->value().length() != (_numberOfLeds * 2))
    {
      request->send(400, F("text/html"), F("Incorrect color size"));
      return;
    }

    const char *colorCodeA = request->getParam(F("color"))->value().c_str();
    //check that color code is hexa
    for (byte i = 0; i < strlen(colorCodeA); i++)
    {
      if (!isHexadecimalDigit(colorCodeA[i]))
      {
        request->send(400, F("text/html"), F("Incorrect color code"));
        return;
      }
    }

    //convert text to byte
    byte colorCode[4];
    for (byte i = 0; i < _numberOfLeds; i++)
    {
      colorCode[i] = Utils::AsciiToHex(colorCodeA[i * 2]) * 0x10 + Utils::AsciiToHex(colorCodeA[i * 2 + 1]);
    }

    SetColor(colorCode);
    request->send(200);
  });

  server.on("/getColor", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, F("text/html"), GetColor());
  });

  server.on("/getJSONColor", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, F("text/html"), GetColor(true));
  });

  server.on("/setTime", HTTP_GET, [this](AsyncWebServerRequest *request) {

    //try to find duration
    if (!request->hasParam(F("time")))
    {
      request->send(400, F("text/html"), F("Missing time parameter"));
      return;
    }

    int time = request->getParam(F("time"))->value().toInt();

    //check time value
    if (!time || time > 100)
    {
      request->send(400, F("text/html"), F("Incorrect time value"));
      return;
    }

    for (byte i = 0; i < _numberOfLeds; i++)
      _leds[i]->setTime(time * 100, true);
    request->send(200);
  });
};

//------------------------------------------
//Run for timer
void WebRGBW::AppRun()
{
  FadeLed::update();
}

//------------------------------------------
//Constructor
WebRGBW::WebRGBW(char appId, String appName) : Application(appId, appName) {}