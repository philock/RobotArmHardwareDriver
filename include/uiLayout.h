// Touchscreen calibration data
#define TS_MINX 379
#define TS_MINY 292
#define TS_MAXX 3915
#define TS_MAXY 3800

#define TFT_WIDTH  320
#define TFT_HEIGHT 240

#define COL_BG          ILI9341_LIGHTGREY // Background color
#define COL_BOX         ILI9341_LIGHTGREY // Box color
#define COL_BUT_PRESSED ILI9341_DARKGREY  // Button pressed color
#define COL_OUTLINE     ILI9341_BLACK     // Outline color for buttons

#define PADDING_SIDES      10 // Padding between UI elemts
#define CORNER_RADIUS      8  // Radius for rounded corners

// Buttons
constexpr int hButton      = (TFT_HEIGHT - 5*PADDING_SIDES)/4;
constexpr int wButtonBig   = 2*hButton;
constexpr int wButtonSmall = hButton;

// Log box
constexpr int wLog = (TFT_WIDTH - 3*PADDING_SIDES - wButtonSmall);
constexpr int hLog = (TFT_HEIGHT - 4*PADDING_SIDES - 2*hButton);
constexpr int xLog = (wButtonSmall + 2*PADDING_SIDES);
constexpr int yLog = (3*PADDING_SIDES + 2*hButton);
constexpr int wScrollbar   = 11; // odd number!
constexpr int padScrollbar = 10; // padding between scrollbar and log box. Also triangle padding in up/down buttons

// Log box text properties
constexpr int fontHight    = 10;
constexpr int linesPadding = 5;
constexpr int nLines = (hLog - linesPadding)/(fontHight + linesPadding);

// Robot arm graphic box
constexpr int wArmBox = (TFT_WIDTH - 3*PADDING_SIDES - wButtonBig);
constexpr int hArmBox = hLog;
constexpr int xArm    = (2*PADDING_SIDES + wButtonBig); 
constexpr int yArm    = PADDING_SIDES; 

// Robot arm graphic dot properties (Status indicators)
constexpr int rDot = 6;

constexpr int xR = xArm + (wArmBox)/2 + 38;
constexpr int yR = yArm + 93;

constexpr int xA = xArm + (wArmBox)/2 + 30;
constexpr int yA = yArm + 73;

constexpr int xB = xArm + (wArmBox)/2 - 23;
constexpr int yB = yArm + 10;

constexpr int xC = xArm + 29;
constexpr int yC = yArm + 62;

constexpr int xD = xArm + 29;
constexpr int yD = yArm + 78;

constexpr int xG = xArm + 29;
constexpr int yG = yArm + 97;
