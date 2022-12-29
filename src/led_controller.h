#pragma once

#include "Arduino.h"
#include <FastLED.h>
#include "RGBStructs.h"

class led_controller {
  public:
    struct rgb {
      uint8_t r;
      uint8_t g;
      uint8_t b;
    } rgb;
    led_controller(int NUM_LEDS, int LED_PIN, CRGB leds[]);
    void    color(CRGB leds[], CRGB color);
    void    staticPattern(CRGB leds[], std::vector<TIP_RGB> pattern);
    void    rainbowScroll(CRGB leds[], int rate, int width, int iterations);
    void    debugPattern(CRGB leds[]);
    void    scrollPattern(CRGB leds[], int rate, int width, int iterations, CRGB[], int p_size);
    void    xyBriToRGB(float x, float y, int bri);

    // Check if two patterns contain the same data, and thus shouldn't be overwritten
    bool    checkRGBPatternEquality(std::vector<TIP_RGB> oldData, std::vector<TIP_RGB> newData);
    
  private:

    // Local RGB Pattern - Used for checking if pattern has changed
    std::vector<TIP_RGB>    _localRGBPattern;


    // Key for driving RGB strips
    int       _LED_PIN;
    int       _NUM_LEDS;
    uint32_t  _roygbiv[7] = {CRGB::Red, CRGB::DarkOrange, CRGB::Yellow, CRGB::DarkGreen, CRGB::Blue, CRGB::Indigo, CRGB::Violet};
};