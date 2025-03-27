#include <ui.h>

void UI::init(){
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(COL_BG);

    ts.begin(SPI);

    for(int i = 0; i < 6; i++) _jointStateIndicators[i] = JointStates::STOP;

    drawButtonHoming();
    drawButtonStartStop(true, false);
    drawButtonUp();
    drawButtonDown();
    drawArmBox();
    drawLogFull();
}

// Read touchscreen and handle inputs
void UI::update(){
    // Update log
    if(logger.newMessagesAvailable()){
        drawLogUpdate();
    }

    // read touchscreen input and detect rising and falling edges
    bool tsState = ts.touched();
    if(_tsTouched != tsState){
        _tsTouched = tsState; // state change detected
    }
    else return;

    if(_tsTouched) handleTsTouch();
    else           handleTsRelease();
}

void UI::handleTsTouch(){
    TS_Point p = ts.getPoint();

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    constexpr int yHitbox = hButton + PADDING_SIDES;

    // ignore right side of the screen
    if(p.x > wButtonBig) return;

    // 0: Home, 1: Start/Stop, 2: Up, 3: Down
    int yIdx = p.y/yHitbox;

    switch (yIdx){
    case 0: // Home
        _homeButtonPressed = true;
        drawButtonHoming(true);
        _cbButtonHome();
        break; 
    case 1: // Start/Stop
        _startStopButtonPressed = true;
        drawButtonStartStop(_startStopButtonIsStart, true);
        _cbButtonStartStop();
        break;
    case 2: // Up
        _upButtonPressed = true;
        drawButtonUp(true);
        scrollUp();
        break;
    case 3: // Down
        _downButtonPressed = true;
        drawButtonDown(true);
        scrollDown();
        break;
    default:
        break;
    }
}

void UI::handleTsRelease(){
    if(_homeButtonPressed){
        _homeButtonPressed = false;
        drawButtonHoming(false);
    }
    if(_startStopButtonPressed){
        _startStopButtonPressed = false;
        drawButtonStartStop(_startStopButtonIsStart, false);
    }
    if(_upButtonPressed){
        _upButtonPressed = false;
        drawButtonUp(false);
    }
    if(_downButtonPressed){
        _downButtonPressed = false;
        drawButtonDown(false);
    }
}


// Set color joint state indicator dot. 0: Red, 1: Orange, 2: Green
void UI::setJointIndicator(const int joint, const JointStates state){
    if(state == _jointStateIndicators[joint]) return;
    
    _jointStateIndicators[joint] = state;

    uint16_t col;

    switch(state){
        case JointStates::ESTOP: 
            col = ILI9341_RED;
            break;
        case JointStates::STOP:
            col = ILI9341_ORANGE;
            break;
        default: // Active and Homing
            col = ILI9341_GREEN;
            break;
    }

    drawDot(xDot[joint], yDot[joint], rDot, col, axisLabels[joint]);
}

// Set wether button should show start or stop
void UI::setStartStopButtonState(bool start){
    _startStopButtonIsStart = start;
    drawButtonStartStop(_startStopButtonIsStart, _startStopButtonPressed);
}

void UI::registerCbButtonHome(CallbackFunction f){
    _cbButtonHome = f;
}

void UI::registerCbButtonStartStop(CallbackFunction f){
    _cbButtonStartStop = f;
}

// Adapted function from adafruit GFX library to support transparency
void UI::drawRGBBitmapTransp(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t transp, int16_t w, int16_t h) {
    //tft.startWrite();

    for (int16_t j = 0; j < h; j++, y++){
      for (int16_t i = 0; i < w; i++){

        uint16_t pixel = bitmap[j * w + i];

        if (pixel != transp) {
            tft.drawPixel(x + i, y, pixel);
        }

      }
    }

    //tft.endWrite();
}

void UI::drawButtonBox(const int x, const int y, const int w, const int h, const bool pressed){
    if(pressed) tft.fillRoundRect(x, y, w, h, CORNER_RADIUS, COL_BUT_PRESSED);
    else        tft.fillRoundRect(x, y, w, h, CORNER_RADIUS, COL_BOX);  

    tft.drawRoundRect(x, y, w, h, CORNER_RADIUS, COL_OUTLINE);
}

void UI::fillLog(){
    const StatusMessage* msg;
  
    for (int i = 0; i < nLines; i++){
        msg = logger.getMessage(i + _scrollPos);
    
        if (msg == nullptr) return;
    
        switch(msg->level){
            case MessageLevel::INFO:
            tft.setTextColor(ILI9341_DARKGREEN);
            //tft.setTextColor(ILI9341_GREEN);
            break;
            case MessageLevel::WARNING:
            //tft.setTextColor(ILI9341_ORANGE);
            tft.setTextColor(ILI9341_YELLOW);
            break;
            case MessageLevel::ERROR:
            tft.setTextColor(ILI9341_RED);
            break;
        }
    
        tft.setFont(LiberationSans_11);
        tft.setCursor(xLog + linesPadding, yLog + hLog - 1 - (i + 1)*(fontHight + linesPadding));
        tft.println(msg->message);

    }
}

// Position: float between 0 and 1
void UI::drawScrollbar(float pos){
    constexpr int h = hLog - 2*padScrollbar;
    constexpr int x = TFT_WIDTH - PADDING_SIDES - padScrollbar - wScrollbar;
    constexpr int y = yLog + padScrollbar;
    constexpr int point_r = wScrollbar/2 - 1;
    constexpr int point_x = x + wScrollbar/2;
    constexpr int point_y_0 = y + point_r;
  
    tft.fillRect(x, y, wScrollbar, h, COL_BG);
    tft.drawRoundRect(x, y, wScrollbar, h, wScrollbar/2, COL_OUTLINE);  
  
    int point_y = point_y_0 + (h - 2*point_r - 1)*(1 - pos);
  
    tft.fillCircle(point_x, point_y, point_r, COL_OUTLINE);
}

void UI::drawLogFull(){
    int msgCount = logger.getMessageCount();
    
    // Draw box
    tft.fillRoundRect(xLog, yLog, wLog, hLog, CORNER_RADIUS, COL_BOX);
    tft.drawRoundRect(xLog, yLog, wLog, hLog, CORNER_RADIUS, COL_OUTLINE);
  
    // Draw scrollbar
    if(msgCount > nLines) drawScrollbar((float)_scrollPos/(msgCount - nLines));
    else                  drawScrollbar(0);
  
    // write text lines
    fillLog();
}

void UI::drawLogUpdate(){
    constexpr int x = xLog + linesPadding;
    constexpr int y = yLog + linesPadding;
    constexpr int w = wLog - 2*linesPadding - padScrollbar - wScrollbar;
    constexpr int h = hLog - 2*linesPadding + 2;
    
    // Clear log box
    tft.fillRect(x, y, w , h, COL_BOX);
    
    // Update scrollbar
    int msgCount = logger.getMessageCount();
    if(msgCount > nLines) drawScrollbar((float)_scrollPos/(msgCount - nLines));
    else                  drawScrollbar(0);
  
    // write text lines
    fillLog();
}

void UI::drawButtonUp(bool pressed){
    constexpr int x = PADDING_SIDES;
    constexpr int y = 3*PADDING_SIDES + 2*hButton;
  
    drawButtonBox(x, y, wButtonSmall, hButton, pressed);
  
    //      0
    //    1   2
  
    // y2 > y1 > y0
    constexpr int y0 = y + padScrollbar;
    constexpr int y1 = y + hButton - padScrollbar;
    constexpr int y2 = y1;
  
    constexpr int x0 = x + wButtonSmall/2;
    constexpr int x1 = x + padScrollbar;
    constexpr int x2 = x + wButtonSmall - padScrollbar;
    tft.fillTriangle(x0, y0, x1, y1, x2, y2, COL_OUTLINE);
}
  
void UI::drawButtonDown(bool pressed){
    constexpr int x = PADDING_SIDES;
    constexpr int y = 4*PADDING_SIDES + 3*hButton;
  
    drawButtonBox(x, y, wButtonSmall, hButton, pressed);
  
    //    0   1
    //      2
  
    // y2 > y1 > y0
    constexpr int y0 = y + padScrollbar;
    constexpr int y1 = y0;
    constexpr int y2 = y + hButton - padScrollbar;;
  
    constexpr int x0 = x + padScrollbar;
    constexpr int x1 = x + wButtonSmall - padScrollbar;
    constexpr int x2 = x + wButtonSmall/2;
    tft.fillTriangle(x0, y0, x1, y1, x2, y2, COL_OUTLINE);
}

void UI::scrollUp(){
    if(_scrollPos + nLines >= logger.getMessageCount()) return;
    _scrollPos ++;
  
    drawLogUpdate();
}
  
void UI::scrollDown(){
    if(_scrollPos == 0) return;
    _scrollPos --;
  
    drawLogUpdate();
}

void UI::drawDot(const int x, const int y, const int r, const uint16_t color, const char letter){
    const int xt = x - r/2 + 1;
    const int yt = y - r/2;
  
    tft.fillCircle(x, y, r, color);
    tft.drawCircle(x, y, r, COL_OUTLINE);
    
    //tft.setFont(LiberationSans_8);
    tft.setFontAdafruit();
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(xt, yt);
    tft.print(letter);
}
  
void UI::drawArmBox(){
    drawRGBBitmapTransp(xArm, yArm, Graphic, Graphic[0], GRAPHIC_WIDTH, GRAPHIC_HEIGHT);
  
    const uint16_t col = ILI9341_RED;

    for(int i = 0; i < 6; i++){
        drawDot(xDot[i], yDot[i], rDot, col, axisLabels[i]);    
    }
} 

void UI::drawButtonStartStop(bool start, bool pressed){
    constexpr int x = PADDING_SIDES;
    constexpr int y = 2*(PADDING_SIDES) + hButton;
    constexpr int w = wButtonBig;
    constexpr int h = hButton;
  
    drawButtonBox(x, y, w, h, pressed);

    tft.setTextColor(COL_OUTLINE);
    tft.setFont(LiberationSans_18);

    if(start){
        tft.setCursor(x + (w / 2) - (strlen("Start") * 18)/2 + 19,
                      y + (h / 2) - (LiberationSans_18.cap_height)/2);
        tft.println("Start");
    }
    else{
        tft.setCursor(x + (w / 2) - (strlen("Stop") * 18)/2 + 10,
                      y + (h / 2) - (LiberationSans_18.cap_height)/2);
        tft.println("Stop");
    }
}
  
void UI::drawButtonHoming(bool pressed){
    constexpr int x = PADDING_SIDES;
    constexpr int y = PADDING_SIDES;
    constexpr int w = wButtonBig;
    constexpr int h = hButton;
  
    drawButtonBox(x, y, w, h, pressed);
  
    tft.setTextColor(COL_OUTLINE);
    tft.setFont(LiberationSans_18);

    tft.setCursor(x + (w / 2) - (strlen("Home") * 18)/2 + 2,
                  y + (h / 2) - (LiberationSans_18.cap_height)/2);
    tft.println("Home");
}