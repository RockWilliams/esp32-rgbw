/*
  This example will receive multiple universes via Art-Net and control a strip of
  WS2812 LEDs via the FastLED library: https://github.com/FastLED/FastLED
  This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

// esp32 board manager link http://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#include <ArtnetWifi.h>
#include <Arduino.h>
#include <FastLED.h>
#include "FastLED_RGBW.h"

// Wifi settings
const char* ssid = "NETGEAR6BE4CF-SETUP";
const char* password = "sharedsecret";

// set static ip address
IPAddress local_IP(6, 10, 10, 235);
// set gateway ip address
IPAddress gateway(6, 10, 10, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// LED settings
const int numLeds = 96; // CHANGE FOR YOUR SETUP
const int numberOfChannels = numLeds * 4; // = 240  Total number of channels you want to receive (1 led = 4 channels)
const byte dataPin = 13;
//FastLED with RGBW
CRGBW leds[numLeds];
CRGB *ledsRGB = (CRGB *) &leds[0];

// Art-Net settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

bool sendFrame = 1;
int previousDataLength = 0;


// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}


void initTest()
{
  for (int i = 0 ; i < 60 ; i++) {
    leds[i] = CRGBW(127, 0, 0, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < 60 ; i++) {
    leds[i] = CRGBW(0, 127, 0, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < 60 ; i++) {
    leds[i] = CRGBW(0, 0, 127, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < 60 ; i++) {
    leds[i] = CRGBW(0, 0, 0, 127);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < 60 ; i++) {
    leds[i] = CRGBW(0, 0, 0, 0);
  }
  FastLED.show();
}



void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{

  sendFrame = 1;
  // set brightness of the whole strip
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
    FastLED.show();
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < 96; i++)// for (int i = 0; i < length / 4; i++)
  {
    leds[i] = CRGBW(data[i * 4 + 1], data[i * 4], data[i * 4 + 2], data[i * 4 + 3]);   // data[i * 4] is the first of the 4 channels per led and so on, Have to swap the first and second data value
  }

  if (sendFrame)
  {
    FastLED.show();
  }
}


void setup()
{
  Serial.begin(115200);
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  ConnectWifi();
  artnet.begin();
  // FastLED with RGBW
  FastLED.addLeds<WS2812, dataPin, RGB>(ledsRGB, getRGBWsize(numLeds));
  //  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  if(WiFi.status() != WL_CONNECTED) {
    ConnectWifi();
  }
  // we call the read function inside the loop
  artnet.read();
}
