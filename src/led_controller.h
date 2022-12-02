#pragma once

#include "Arduino.h"
#include <FastLED.h>

class led_controller {
  public:
    struct rgb {
      uint8_t r;
      uint8_t g;
      uint8_t b;
    } rgb;
    led_controller(int NUM_LEDS, int LED_PIN, CRGB leds[]);
    void color(CRGB leds[], CRGB color);
    void rainbowScroll(CRGB leds[], int rate, int width, int iterations);
    void debugPattern(CRGB leds[]);
    void scrollPattern(CRGB leds[], int rate, int width, int iterations, CRGB[], int p_size);
    void xyBriToRGB(float x, float y, int bri);
    
  private:
    int       _LED_PIN;
    int       _NUM_LEDS;
    uint32_t  _roygbiv[7] = {CRGB::Red, CRGB::DarkOrange, CRGB::Yellow, CRGB::DarkGreen, CRGB::Blue, CRGB::Indigo, CRGB::Violet};
};