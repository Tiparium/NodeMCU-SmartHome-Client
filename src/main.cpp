/*
  ==============================================================================

    Main.cpp
    Created: 1 Dec 2022 5:22:??pm
    Author:  Tiparium

    Main

  ==============================================================================
*/

// Mandatory
#include <Arduino.h>

// LED Control
#include <Adafruit_NeoPixel.h>
#include <FastLED.h> // <- May be removed

// Local Code
#include "WebClient.h"

WebClient _webClient;

void setup()
{
    _webClient.connectToWifi();
    _webClient.initLEDS();
}

void loop()
{
    delay(100);
    _webClient.syncLEDS();
}