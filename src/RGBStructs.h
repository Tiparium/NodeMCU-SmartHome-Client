/*
  ==============================================================================

    RGBStructs.h
    Created: 12 Oct 2022 6:03:05pm
    Author:  Tiparium

    Contains structs representing color data in a few different formats.
    NodeMCU LEDS take RGB, while pHue LEDS take XYB

    This version has been modified from the version present in my JUCE projects.
    TODO: Remove the extra bloat present here (There's a lot)
  ==============================================================================
*/

#pragma once

// Mandatory
#include <Arduino.h>

// I definitely didn't steal these structs. Nope. (Credit to Moritz Wirger, @enwi on github)
// Some of my own modifications have been added. And again for conversion to NodeMCU.
struct TIP_XY
{
    float x;
    float y;

    bool operator==(const TIP_XY& other) const { return x == other.x && y == other.y; }
    bool operator!=(const TIP_XY& other) const { return !(*this == other); }
};
struct TIP_XYB
{
    TIP_XY xy;
    float brightness;

    bool operator==(const TIP_XYB& other) const { return xy == other.xy && brightness == other.brightness; }
    bool operator!=(const TIP_XYB& other) const { return !(*this == other); }
};
struct TIP_RGB
{
    uint8 r;
    uint8 g;
    uint8 b;
    // Initialize all values to 0
    TIP_RGB() {
        r = 0;
        g = 0;
        b = 0;
    }
    // Initialize all values to values
    TIP_RGB(uint8 rIn, uint8 gIn, uint8 bIn) {
        r = rIn;
        g = gIn;
        b = bIn;
    }

    bool operator==(const TIP_RGB& other) const { return r == other.r && g == other.g && b == other.b; }
    bool operator!=(const TIP_RGB& other) const { return !(*this == other); }

    // Converting between formats
    TIP_XYB toXY() const
    {
        if (r == 0 && g == 0 && b == 0)
        {
            // Return white with minimum brightness
            return TIP_XYB{ TIP_XY {0.32272673f, 0.32902291f}, 0.f };
        }
        const float red = r / 255.f;
        const float green = g / 255.f;
        const float blue = b / 255.f;

        const float redCorrected = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
        const float greenCorrected = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
        const float blueCorrected = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

        const float X = redCorrected * 0.664511f + greenCorrected * 0.154324f + blueCorrected * 0.162028f;
        const float Y = redCorrected * 0.283881f + greenCorrected * 0.668433f + blueCorrected * 0.047685f;
        const float Z = redCorrected * 0.000088f + greenCorrected * 0.072310f + blueCorrected * 0.986039f;

        const float x = X / (X + Y + Z);
        const float y = Y / (X + Y + Z);
        // Set brightness to the brightest channel value (rather than average of them),
        // so full red/green/blue can be displayed
        return TIP_XYB{ TIP_XY {x, y}, std::max({red, green, blue}) };
    }
    TIP_RGB fromXY(const TIP_XYB& xy)
    {
        if (xy.brightness < 1e-4)
        {
            return TIP_RGB{ 0, 0, 0 };
        }
        const float z = 1.f - xy.xy.x - xy.xy.y;
        // use a fixed luminosity and rescale the resulting rgb values using brightness
        // randomly sampled conversions shown a minimum difference between original values
        // and values after rgb -> xy -> rgb conversion for Y = 0.3
        // (r-r')^2, (g-g')^2, (b-b')^2:
        // 4.48214,  4.72039,  3.12141
        // Max. Difference:
        // 9,        9,        8
        const float Y = 0.3f;
        const float X = (Y / xy.xy.y) * xy.xy.x;
        const float Z = (Y / xy.xy.y) * z;

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
            // Low color values, out of gamut?
            return TIP_RGB{ 0, 0, 0 };
        }
        const float rScaled = gammaR / maxColor * xy.brightness * 255.f;
        const float gScaled = gammaG / maxColor * xy.brightness * 255.f;
        const float bScaled = gammaB / maxColor * xy.brightness * 255.f;

        return TIP_RGB{ static_cast<uint8_t>(std::round(std::max(0.f, rScaled))),
            static_cast<uint8_t>(std::round(std::max(0.f, gScaled))),
            static_cast<uint8_t>(std::round(std::max(0.f, bScaled))) };
    }

    void setVals(uint8 r, uint8 g, uint8 b) {
        r = r;
        g = g;
        b = b;
    }

    // For displaying accurate colors on screen
    TIP_RGB colorCorrect() {
        if (r == 0 && g == 0 && b == 0) { return TIP_RGB(255, 255, 255); }
        TIP_RGB out = TIP_RGB(r, g, b);
        while (out.r < 255 && out.g < 255 && out.b < 255) {
            if (r > 0) { out.r++; }
            if (g > 0) { out.g++; }
            if (b > 0) { out.b++; }
        }
        return out;
    }
};