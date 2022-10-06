#include "Arduino.h"
#include "../src/led_controller.h"
led_controller::led_controller(int NUM_LEDS, int LED_PIN, CRGB leds[]) {
  _LED_PIN  = LED_PIN;
  _NUM_LEDS = NUM_LEDS;
}

// Displays one solid color
void led_controller::color(CRGB leds[], CRGB color){
  for(int i = 0; i < _NUM_LEDS; i++){
    leds[i] = color;
  }
  FastLED.show();
}

void led_controller::rainbowScroll(CRGB leds[],int rate, int width, int iterations){
  CRGB pattern[7];
  for(int i = 0; i < 7; i++){
    pattern[i] = _roygbiv[i];
  }
  led_controller::scrollPattern(leds, rate, width, iterations, pattern, 7);
}

void led_controller::debugPattern(CRGB leds[]){
  CRGB pattern[3] = {CRGB::Red, CRGB::DarkGreen, CRGB::Black};
  led_controller::scrollPattern(leds, 0, 1, 1, pattern, 3);
}

//(w, x, y, z, a, b)
// w: LEDS
// x: RATE - Set to -1 for Stationary Pattern, Recommended min: 50
// y: WIDTH
// z: REPS
// a: INPUT PATTERN
// b: INPUT PATTERN SIZE
void led_controller::scrollPattern(CRGB leds[],int rate, int width, int iterations, CRGB pattern[], int p_size){
  int index = 0;
  int color = 0;
  for(int i = 0; i < _NUM_LEDS; i++){
    leds[i] = pattern[color];
    index++;
    if(index == width){
      index = 0;
      color++;
    }
    if(color == p_size)color = 0;    
  }
  FastLED.show();
  if(rate == -1)return;
  delay(rate);
  for(int x = 0; x < _NUM_LEDS * iterations; x++){
    CRGB front = leds[0];
    for(int i = 0; i < _NUM_LEDS; i++){
      leds[i] = leds[i + 1];
    }
    leds[_NUM_LEDS - 1] = front;
    FastLED.show();
    delay(rate);
  }
}

void led_controller::xyBriToRGB(float x, float y, int bri){
    bri = 1;
    bool debug = true;
    float z = 1.0f - x - y;
    float Y = bri;
    float X = (Y / y) * x;
    float Z = (Y / y) * z;

    float r =  X * 1.4628067f - Y * 0.1840623f - Z * 0.2743606f;
    float g = -X * 0.5217933f + Y * 1.4472381f + Z * 0.0677227f;
    float b =  X * 0.0349342f - Y * 0.0968930f + Z * 1.2884099f;

    r = r <= 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * pow(r, (1.0f / 2.4f)) - 0.055f;
    g = g <= 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * pow(g, (1.0f / 2.4f)) - 0.055f;
    b = b <= 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * pow(b, (1.0f / 2.4f)) - 0.055f;

    double maxVal = std::max((double)r, (double)g);
    maxVal = std::max(maxVal, (double)b);

    r /= (float)maxVal;
    g /= (float)maxVal;
    b /= (float)maxVal;

    r = r * 255;
    g = g * 255;
    b = b * 255;

    if(r < 0){r = 255;}
    if(g < 0){g = 255;}
    if(b < 0){b = 255;}

    led_controller::rgb.r = (int)r;
    led_controller::rgb.g = (int)g;
    led_controller::rgb.b = (int)b;

    if(debug){
        Serial.print("X: ");
        Serial.println(x);
        Serial.print("Y: ");
        Serial.println(y);
        Serial.print("Bri: ");
        Serial.println(bri);
        Serial.print("R: ");
        Serial.println(r);
        Serial.print("G: ");
        Serial.println(g);
        Serial.print("B: ");
        Serial.println(b);
        Serial.println("-------------------");
    }
}