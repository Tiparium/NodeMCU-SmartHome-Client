#pragma once

#include "WebClient.h"

int WebClient::connectToWifi()
{
    const char* ssid        = _data.ssid.c_str();
    const char* password    = _data.pswd.c_str();
    WiFi.begin(ssid, password);
    Serial.begin(115200);
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".'-");
        delay(60);
    } Serial.println("\n");
    Serial.println("Connecetd to: ");
    Serial.print("  SSID: ");
    Serial.println(ssid);
    Serial.print("  ON IP: " );
    Serial.println(WiFi.localIP());
    return 0;
}

int WebClient::getColorsFromWebServer()
{
    _wifiClient.setTimeout(10000);
    if(!_wifiClient.connect(_ip, 80))
    {
        Serial.println(F("Connection Failed!"));
        return -1;
    }

    // Hold up there Kimosabe
    yield();

    // Send HTTP req
    _wifiClient.print(F("GET "));
    _wifiClient.print(_path);
    _wifiClient.println(F(" HTTP/1.1"));

    // Skip Headers
    _wifiClient.print(F("Host: "));
    _wifiClient.print(_ip);
    _wifiClient.println(F("Cache-Control: no-cache"));

    if(_wifiClient.println() == 0)
    {
        Serial.println(F("Failed To Send Request!"));
        return -2;
    }
    delay(100);

    // Check HTTP Status
    char status[32] = {0};
    _wifiClient.readBytesUntil('\r', status, sizeof(status));
    if(strcmp(status, "HTTP/1.1 200 OK") != 0)
    {
        Serial.print(F("Unexpected Response! See Below."));
        Serial.println(status);
        return -3;
    }

    // Skip HTTP Headers
    char endOfHeaders[] = "\r\n\r\n";
    if(!_wifiClient.find(endOfHeaders))
    {
        Serial.println(F("Invalid Response!"));
        return -4;
    }

    while(_wifiClient.available() && _wifiClient.peek() != '{')
    {
        char c = 0;
        _wifiClient.readBytes(&c, 1);
        Serial.print(c);
        Serial.println("BAD");
    }

    DynamicJsonDocument json(10000);
    DeserializationError err = deserializeJson(json, _wifiClient);

    String debugStr;
    serializeJsonPretty(json, debugStr);
    Serial.println(debugStr);

    if(!err)
    {
        Serial.println("goodData!");
        std::vector<TIP_RGB> patternBuilder;

        for(int i = 0; i < json["rgbDataPoints"].size(); i++)
        {
            TIP_RGB tempRGB;
            String r = json["rgbDataPoints"][i]["r"];
            String g = json["rgbDataPoints"][i]["g"];
            String b = json["rgbDataPoints"][i]["b"];

            tempRGB.r = r.toInt();
            tempRGB.g = g.toInt();
            tempRGB.b = b.toInt();

            patternBuilder.push_back(tempRGB);
        }
        _rgbPattern = patternBuilder;
    } else {
        Serial.println("Bad Data!");
    }

    bool debug = false;

    if(debug)
    {
        debugPrintRGBPattern();
    }

    return 0;
}

void WebClient::initLEDS()
{
    // Initialize LEDS & set to debug state
    FastLED.addLeds<WS2812B, _led_pin, GRB>(_leds, _numLEDS);
    FastLED.setBrightness(45);
    _ledController.debugPattern(_leds);
}

void WebClient::syncLEDS(){
    _ledController.staticPattern(_leds, _rgbPattern);
}

// DEBUG
void WebClient::debugPrintRGBPattern()
{
    for(size_t i = 0; i < _rgbPattern.size(); i++)
    {
        Serial.print("Item: ");
        Serial.println(i);
        Serial.print("R: ");
        Serial.println(_rgbPattern[i].r);
        Serial.print("G: ");
        Serial.println(_rgbPattern[i].g);
        Serial.print("B: ");
        Serial.println(_rgbPattern[i].b);
        Serial.println("---------------------------------");
    }
    Serial.println("****_________________________________****");

}