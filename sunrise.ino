#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include "time.h"

const char* ssid = "";
const char* password   = "";

const char* TZ = "CET-1CEST,M3.5.0,M10.5.0/3";

const char* ntpServer = "europe.pool.ntp.org";
//const long  gmtOffset_sec = 3600;
//const int   daylightOffset_sec = 0;//3600;


#define LED_PIN     15
#define NUM_LEDS    4
#define WAKE_HOUR 6
#define WAKE_MIN 30
#define SLEEP_HOUR 19
#define SLEEP_MIN 0
CRGB leds[NUM_LEDS];


void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);  
  LEDS.setBrightness(100);
  
  initTime();
}
void loop() {
  struct tm tm;
  getLocalTime(&tm);
  Serial.println(&tm, "%H:%M:%S");

  if (checkTime(tm)) {
    //green
    leds[0] = CRGB(0, 255,0);
  leds[1] = CRGB(0, 255,0);
  leds[2] = CRGB(0,255,0);
  leds[3] = CRGB(0,255,0);
  
  } else {
    //red
    leds[0] = CRGB(255,0,0);
  leds[1] = CRGB(255,0,0);
  leds[2] = CRGB(255,0,0);
  leds[3] = CRGB(255,0,0);
  }
  
  FastLED.show();
  delay(1000);
//
//
//  fadeTowardColor(leds[0], CRGB(0,255,0), 1);
//  fadeTowardColor(leds[1], CRGB(0,255,0), 1);
//  fadeTowardColor(leds[2], CRGB(0,255,0), 1);
//  fadeTowardColor(leds[3], CRGB(0,255,0), 1);
//  FastLED.show();
//  delay(50);
  
}

boolean checkTime(tm tm) {
  boolean wake = false;
  if (tm.tm_hour > WAKE_HOUR && tm.tm_hour < SLEEP_HOUR) {
    wake = true;
  } else if ((tm.tm_hour == WAKE_HOUR && tm.tm_min >= WAKE_MIN) || (tm.tm_hour == SLEEP_HOUR && tm.tm_min <= SLEEP_MIN)) {
    wake = true;
  }

  return wake;
}

void initTime() {
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  configTzTime(TZ, ntpServer);

  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void printLocalTime()
{
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;

  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}
