#include "Arduino.h"
#include "../src/led_controller.h"
led_controller::led_controller(int NUM_LEDS, int LED_PIN, CRGB leds[]) {
    _LED_PIN  = LED_PIN;
    _NUM_LEDS = NUM_LEDS;
}

// Displays one solid color
void led_controller::color(CRGB leds[], CRGB color){
    for(int i = 0; i < _NUM_LEDS; i++)
    {
        leds[i] = color;
    }
    FastLED.show();
}

void led_controller::staticPattern(CRGB leds[], std::vector<CRGB> pattern)
{
    int patternIndex = 0;
    for(int i = 0; i < _NUM_LEDS; i++)
    {
        leds[i] = pattern[patternIndex];

        patternIndex++;
        if(patternIndex == pattern.size())
        {
            patternIndex = 0;
        }
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
    bool debug = true;
    if (bri < 1e-4) {
        led_controller::rgb.r = 0;
        led_controller::rgb.g = 0;
        led_controller::rgb.b = 0;
        return;
    }
    const float z = 1.f - x - y;

    const float Y = 0.3f;
    const float X = (Y / y) * x;
    const float Z = (Y / y) * z;

    const float r = X * 1.656492f - Y * 0.354851f - Z * 0.255038f;
    const float g = -X * 0.707196f + Y * 1.655397f + Z * 0.036152f;
    const float b = X * 0.051713f - Y * 0.121364f + Z * 1.011530f;

    // Reverse gamma correction
    const float gammaR = r <= 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * pow(r, (1.0f / 2.4f)) - 0.055f;
    const float gammaG = g <= 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * pow(g, (1.0f / 2.4f)) - 0.055f;
    const float gammaB = b <= 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * pow(b, (1.0f / 2.4f)) - 0.055f;

    // Scale color values so that the brightness matches
    const float maxColor = std::max({ gammaR, gammaG, gammaB });
    if (maxColor < 1e-4)
    {
        led_controller::rgb.r = 0;
        led_controller::rgb.g = 0;
        led_controller::rgb.b = 0;
        return;
    }
    const float rScaled = gammaR / maxColor * bri * 255.f;
    const float gScaled = gammaG / maxColor * bri * 255.f;
    const float bScaled = gammaB / maxColor * bri * 255.f;

    uint8_t rVal = std::round(std::max(0.f, rScaled));
    uint8_t gVal = std::round(std::max(0.f, gScaled));
    uint8_t bVal = std::round(std::max(0.f, bScaled));
    
    led_controller::rgb.r = rVal;
    led_controller::rgb.g = gVal;
    led_controller::rgb.b = bVal;
    
    if(debug){
        Serial.print("X: ");
        Serial.println(x);
        Serial.print("Y: ");
        Serial.println(y);
        Serial.print("Bri: ");
        Serial.println(bri);
        Serial.print("R: ");
        Serial.println(rgb.r);
        Serial.print("G: ");
        Serial.println(rgb.g);
        Serial.print("B: ");
        Serial.println(rgb.b);
        Serial.println("-------------------");
    }
}