#ifndef _SCROLLTEXT_H
#define _SCROLLTEXT_H
#include "application.h"
class ScrollText {
    public:
        ScrollText(int width, int charSize, float speed);
        void setText(String &text);
        String* getCurrentText();
        int getCurrentOffset();
        void update();
        void start();
    private:
        String* _currentText = NULL;
        String* _text = NULL;
        int _width;
        int _charSize;
        float _speed;
        int _currentOffset;
        unsigned long _start;
        unsigned long _end;
        int _textWidth;
        int _totalWidth;
        bool _needScroll;
        int getTextWidth();
        void start(unsigned long last);
 
};
#endif
