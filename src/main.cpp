#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "../src/led_controller.h"

// Definitions Section ------------------------------------------------------
// LED SETUP ------------------------------
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
//#define NUM_LEDS  8   // -> Mini Strip
#define NUM_LEDS  9  // -> Medi Strip
//#define NUM_LEDS  175 // -> Mess Around
//#define NUM_LEDS  300 // -> Maxi Strip
#define LED_PIN   D2
CRGB leds[NUM_LEDS];
led_controller controller(NUM_LEDS, LED_PIN, leds);
// LED SETUP ------------------------------ END
// WIFI SETUP ----------------------------- (Not Needed for Bluetooth Stuff)
char ssid[] = "DarwinArts";
char pswd[] = "CambrianDialogs";
WiFiClient client;
#define TEST_HOST "10.0.0.139"
// WIFI SETUP ----------------------------- END(Not Needed for Bluetooth Stuff)

char Data;
int g_r;
int g_g;
int g_b;

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

void loading() {
  Serial.print(".");
  delay(500);
}

void connected() {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void setup() {
  // Initialize LEDS & set to debug state
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(45);
  controller.debugPattern(leds);

  // Open serial at given Baud rate
  Serial.begin(115200);

  // Set WiFi to station mode & disconnect from an OP if previously connected
  delay(500);
  clearScreen();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Cleaned Up.");
  delay(500);

  // Attempt to connect to WiFi network
  Serial.print("Connecting WiFi: ");
  Serial.print(ssid);
  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED) {
    loading();
  }
  Serial.println("");
  connected();
}

void makeHTTPRequest() {
  client.setTimeout(10000);
  if (!client.connect(TEST_HOST, 80)) {
    Serial.println(F("Connection Failed"));
    return;
  }
  // Hold up there Kimosabe
  yield();

  // Send HTTP req
  client.print(F("GET "));
  client.print("/api/5Gpo7VxAqssVu2E8lvpcZTRCVzqfmngwBbXqvJpO/lights");
  client.println(F(" HTTP/1.1"));

  // Skip Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);
  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }
  delay(100);

  // Check HTTP Status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid Response"));
    return;
  }
  while (client.available() && client.peek() != '{') {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  DynamicJsonDocument json(6144);
  DeserializationError err = deserializeJson(json, client);
  float xFinal;
  float yFinal;
  float briFinal;
  if (!err) {
    for (JsonPair item : json.as<JsonObject>()) {
      const char* item_key = item.key().c_str();
      JsonObject value_state = item.value()["state"];
      xFinal = value_state["xy"][0];
      yFinal = value_state["xy"][1];
      briFinal = value_state["bri"];
    }
  }

  controller.xyBriToRGB(xFinal, yFinal, briFinal);
  g_r = controller.rgb.r;
  g_g = controller.rgb.g;
  g_b = controller.rgb.b;
}

void syncLEDS(int rate = 1000){
  bool debug = false;
  makeHTTPRequest();
  delay(1000);
  CRGB color = {g_r, g_g, g_b};
  controller.color(leds, color);
  if(debug){
    Serial.print("R: ");
    Serial.println(g_r);
    Serial.print("G: ");
    Serial.println(g_g);
    Serial.print("B: ");
    Serial.println(g_b);
    Serial.println("-------------------");
  }
}

void loop() {
  syncLEDS(200);
}