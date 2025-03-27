#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
//#include <Adafruit_ILI9341.h>
#include <ILI9341_t3.h>
#include <logger.h>
#include <uiGraphics.h>
#include <uiLayout.h>
#include <pinConfig.h>


class UI{
    private:
        ILI9341_t3 tft = ILI9341_t3(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

        typedef void (*CallbackFunction)();
        CallbackFunction _cbButtonHome;
        CallbackFunction _cbButtonStartStop;

        int _scrollPos = 0; // 0 to 20-nLines

        bool _upButtonPressed        = false;
        bool _downButtonPressed      = false;
        bool _startStopButtonPressed = false;
        bool _homeButtonPressed      = false;
        bool _startStopButtonIsStart = true;

        void drawRGBBitmapTransp(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t transp, int16_t w, int16_t h);
        void drawButtonBox(const int x, const int y, const int w, const int h, const bool pressed = false);
        void drawDot(const int x, const int y, const int r, const uint16_t color, const char letter);

        void fillLog();
        void drawLogFull();
        void drawLogUpdate();
        void drawScrollbar(float pos);
        void scrollUp();
        void scrollDown();

        void drawArmBox();

        void drawButtonHoming(bool pressed = false);
        void drawButtonStartStop(bool start, bool pressed = false);
        void drawButtonUp(bool pressed = false);
        void drawButtonDown(bool pressed = false);

    public:
        void init();

        void update();

        void setJointIndicator(const char joint, const int state);

        void setStartStopButtonState(bool start);

        void registerCbButtonHome(CallbackFunction f);
        void registerCbButtonStartStop(CallbackFunction f);
};

#endif