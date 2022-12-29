#pragma once
#include <Arduino.h>

// WebServer
#include <Wire.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// Data
#include "data.h"
#include "RGBStructs.h"
#include "ArduinoUtils.h"
#include "../src/led_controller.h"

#define _led_pin D2

class WebClient
{
public:

    // Wifi Client
    int     connectToWifi();
    int     getColorsFromWebServer();

    //RGB Controller
    void    initLEDS();
    void    syncLEDS();

    // Debug
    void debugPrintRGBPattern();

private:
    // Wifi Client
    data        _data;
    String      _http   = "http://";
    String      _ip     = "10.0.0.104";
    String      _path   = "/getcolor";
    WiFiClient  _wifiClient;

    // RGB Controller
    std::vector<TIP_RGB>    _rgbPattern;
    TIP_RGB          _rgb = TIP_RGB(0, 0, 0);
    static const int _numLEDS = 28; // 9 for microtesting, 300 for whole strip, 175 far large scale testing
    CRGB             _leds[_numLEDS];
    led_controller   _ledController = led_controller(_numLEDS, _led_pin, _leds);
};