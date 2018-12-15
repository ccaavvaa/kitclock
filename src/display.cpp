#include "display.h"

uint8_t contrast[] = {0, 127, 255};
uint8_t precharge[] = {0, 0, 255};

Display::Display() {
  this->_isOn = true;
  this->_brightnessLevel = 0;
}
void Display::start(Adafruit_SSD1306* display){
  this->display = display;
  if (this->_isStarted) {
    return;
  }
  this->display->begin(SSD1306_SWITCHCAPVCC);
  this->_isStarted = true;
  this->setBrightnessLevel(this->_brightnessLevel);

  display->setTextSize(4);
  display->setTextColor(WHITE);
  display->setTextWrap(false);
}

int Display::getBrightnessLevel() {
  return this->_brightnessLevel;
}
void Display::setBrightnessLevel(int value) {
  this->_brightnessLevel = value;
  if(this->_isStarted) {
    this->display->ssd1306_command(SSD1306_SETCONTRAST);
    this->display->ssd1306_command(this->getContrast());
    this->display->ssd1306_command(SSD1306_SETPRECHARGE);
    this->display->ssd1306_command(this->getPrecharge());
  }
}
bool Display::getIsOn() {
  return this->_isOn;
}

void Display::setIsOn(bool value) {
  this->_isOn = value;
  if(this->_isStarted) {
    uint8_t v = this->_isOn ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF;
    this->display->ssd1306_command(v);
  }
}
int Display::getContrast() {
  return contrast[this->_brightnessLevel];
}
int Display::getPrecharge() {
  return precharge[this->_brightnessLevel];
}

int16_t Display::getTextWidth(const String &text, int textSize){
    return text.length() * 6 * textSize;
}
