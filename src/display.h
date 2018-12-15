#ifndef  _Adafruit_SSD1306_H
#define _Adafruit_SSD1306_H
#include <Adafruit_SSD1306.h>
#endif

#ifndef _DISPLAY_H
#define _DISPLAY_H

class Display {
public:
  Display();
  void start(Adafruit_SSD1306* display);

  int getBrightnessLevel();
  void setBrightnessLevel(int value);
  bool getIsOn();
  void setIsOn(bool value);
  int getContrast();
  int getPrecharge();
  int16_t getTextWidth(const String &text, int textSize);

private:
  Adafruit_SSD1306* display;
  bool _isStarted = false;
  bool _isOn;
  int _brightnessLevel;
};
#endif
