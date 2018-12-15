#include "scrolltext.h"
ScrollText::ScrollText(int width, int charSize, float speed) {
    _width = width;
    _charSize = charSize;
    _speed = speed;
}

void ScrollText::setText(String &text) {
    _text = &text;
}

String* ScrollText::getCurrentText() {
    return _currentText; 
}

int ScrollText::getCurrentOffset() {
    return _currentOffset;
}
void ScrollText::update() {
    unsigned long now = millis();
    if(now > _end) {
        start(now);
    }
    if(_needScroll){
        unsigned long delay = now - _start;
        _currentOffset = _width - (int) (delay * _speed);
    }
}
void ScrollText::start() {
    start(millis());
}
void ScrollText::start(unsigned long last) {
    _start = last;
    _textWidth = getTextWidth();
    _needScroll = _textWidth > _width;
    if(_needScroll) {
        _totalWidth = _width + _textWidth;
        int timeOffset = static_cast<unsigned long>(_totalWidth / _speed);
        _end = _start + timeOffset;
        _currentOffset = _width;
    } else {
        _currentOffset = (_width - _textWidth)/2;
    }
    _currentText = _text;

}

int ScrollText::getTextWidth() {
    return _text->length() * _charSize *6;
}
