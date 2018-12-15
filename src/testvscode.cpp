#include "application.h"
#line 1 "/Users/cata/p/particle/testvscode/src/testvscode.ino"
#include "scrolltext.h"
#include "display.h"
void setup();
void loop();
void setTime();
int setMessage(String msg);
void setForecastMessage(const char *event, const char *data);
void doForecast();
void noBreathingWhenOK();
#line 3 "/Users/cata/p/particle/testvscode/src/testvscode.ino"
#ifndef  _Adafruit_SSD1306_H
#define _Adafruit_SSD1306_H
#include <Adafruit_SSD1306.h>
#endif
/*
 * Project kitclo
 * Description:
 * Author:
 * Date:
 */

// use hardware SPI
#define OLED_DC     D3
#define OLED_CS     D4
#define OLED_RESET  D5

#define TEMP    D0
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
Display d;
ScrollText s1(128, 4, 0.05);
ScrollText s2(128, 4, 0.05);
String hello = "";
String times;
String dates;
int lastMinute = 61;
unsigned long lastForecastMessage = 0 - 5400*1000;
bool forecastPending = false;

// setup() runs once, when the device is first turned on.
void setup() {
  noBreathingWhenOK();
  pinMode(D1, OUTPUT);

  Particle.function("setMessage", setMessage);
  Particle.subscribe("hook-response/temp", setForecastMessage, MY_DEVICES);
 
  d.start(&display);
  d.setBrightnessLevel(0);
  
  s1.setText(hello);
  s2.setText(hello);
  s1.start();
  s2.start();
  Time.beginDST();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    //ts.update();
    //setTemp();
    setTime();
    s1.update();
    s2.update();
    String* message = s1.getCurrentText();
    int messageOffset = s1.getCurrentOffset();
    String* timeStr = s2.getCurrentText();
    int timeOffset = s2.getCurrentOffset();
    display.clearDisplay();

    display.setCursor(messageOffset,36);
    display.print(*message);
    display.setCursor(timeOffset,0);
    display.print(*timeStr);
    display.display();
    doForecast();
}

void setTime() {
  const int currentTime = Time.now();
  const int m = Time.minute(currentTime);
  if (m != lastMinute)
  {
    char myTimeString[125] = "";
    int h = Time.hour(currentTime);
    sprintf(myTimeString, "%02d:%02d", h, m);
    times = String(myTimeString, 5);
    s2.setText(times);
    lastMinute = m;
    if(h==12 && m==0) {
        Particle.syncTime();
    }
    if(h==12 && m==0) {
        Particle.syncTime();
    }
    int day = Time.day(currentTime);
    int month = Time.month(currentTime);
    sprintf(myTimeString, "%02d/%02d", day, month);
    dates = String(myTimeString, 5);
    if(hello.length()==0) {
        s1.setText(dates);
    }
  };
}
int setMessage(String msg) {
    hello = msg;
    if(hello.length()==0){
        s1.setText(dates);
    } else {
        tone(D1, 100, 50);

        s1.setText(hello);
    }
    return 1;
}

void setForecastMessage(const char *event, const char *data) {
  lastForecastMessage = millis();
  forecastPending = false;
  String s(data);
  setMessage(s);
}

void doForecast() {
    unsigned long now = millis();
    if(forecastPending) {
        if (now - lastForecastMessage > 60000) {
            lastForecastMessage = now;
            Particle.publish("temp", "", PRIVATE);
            return;
        }
    }
    if(now - lastForecastMessage > 5400 * 1000) {
        lastForecastMessage = now;
        forecastPending = true;
        Particle.publish("temp", "", PRIVATE);
    }
}

/*
 * No green breathing led if ok
 */

void noBreathingWhenOK() {
  LEDSystemTheme theme;
  theme.setColor(LED_SIGNAL_CLOUD_CONNECTED, 0x00000000); // Set LED_SIGNAL_NETWORK_ON to no color
  theme.apply(); // Apply theme settings
}


