#pragma once
#include <Arduino.h>

struct Utils {

    void clearScreen() {
        delay(200);
        Serial.println("");
        Serial.print("Cleaning Up.");
        for (int i = 0; i < 25; i++) {
            Serial.print(".");
            delay(60);
        }

        for (int i = 0; i < 210; i++) {
            Serial.println("\n");
        }
    }
    
};