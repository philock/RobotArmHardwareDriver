#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>
#include <logger.h>
#include <uiGraphics.h>
#include <uiLayout.h>
#include <pinConfig.h>
#include <systemConfig.h>
#include <font_LiberationSans.h>

class UI{
    private:
        ILI9341_t3          tft = ILI9341_t3(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
        XPT2046_Touchscreen ts  = XPT2046_Touchscreen(PIN_TOUCH_CS);

        typedef void (*CallbackFunction)();
        CallbackFunction _cbButtonHome;
        CallbackFunction _cbButtonStartStop;

        int _scrollPos = 0; // 0 to (20-nLines)

        bool _tsTouched              = false;

        bool _upButtonPressed        = false;
        bool _downButtonPressed      = false;
        bool _startStopButtonPressed = false;
        bool _homeButtonPressed      = false;
        
        bool _startStopButtonIsStart = true;

        JointStates _jointStateIndicators[6];

        // Draw Bitmap and skip pixels matching transparency color
        void drawRGBBitmapTransp(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t transp, int16_t w, int16_t h);

        // Draw empty button box
        void drawButtonBox(const int x, const int y, const int w, const int h, const bool pressed = false);

        // Draw dot with letter inside, used for joint state indicators
        void drawDot(const int x, const int y, const int r, const uint16_t color, const char letter);

        // Draw log text inside the log box
        void fillLog();

        // Draw full log with box and text
        void drawLogFull();

        // Clear log box and redraw text
        void drawLogUpdate();

        // Draw scrollbar with pos between 0 and 1
        void drawScrollbar(float pos);

        // scroll log box up or down
        void scrollUp();
        void scrollDown();

        // Draw bitmap ob robot arm and add joint-state indicator dots
        void drawArmBox();

        // handle button activations: dark shading of pressed button and call of activation handler
        void handleTsTouch();

        // handle button deactivations: light shading of released button
        void handleTsRelease();

        // Button drawing functions
        void drawButtonHoming(bool pressed = false);
        void drawButtonStartStop(bool start, bool pressed = false);
        void drawButtonUp(bool pressed = false);
        void drawButtonDown(bool pressed = false);

    public:
        void init();

        // Called periodically to draw new log messages and handle touch inputs
        void update();

        // set joint indicators: joint: 0-R, 1-A, 2-B, 3-C, 4-D, 5-G 
        void setJointIndicator(const int joint, const JointStates state);

        void setStartStopButtonState(bool start);

        void registerCbButtonHome(CallbackFunction f);
        void registerCbButtonStartStop(CallbackFunction f);
};

#endif