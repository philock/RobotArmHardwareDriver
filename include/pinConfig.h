#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

// Emergency stop
#define PIN_EMS 2

// Power supply relay
#define PIN_RELAY 27

struct AxisConfig{
    int pinStep;
    int pinDir;
    int pinCs; // Chip select
    int pinEn; // Enable
    int pinEs; // Endstop

    float transmissionFactor;
    float degPerFullStep;
    int microsteps;
    int spiAddress;

    bool endstopInputPullup;
    bool endstopActiveHigh;

    Stream* uartSerialPort;
};


// Axis A
#define PIN_AX_A_STP 40
#define PIN_AX_A_DIR 35
#define PIN_AX_A_CS  0
#define PIN_AX_A_EN  39
#define PIN_AX_A_ES  5
const AxisConfig axisConfigA = {
    .pinStep = PIN_AX_A_STP,
    .pinDir  = PIN_AX_A_DIR,
    .pinCs   = PIN_AX_A_CS,
    .pinEn   = PIN_AX_A_EN,
    .pinEs   = PIN_AX_A_ES,
    .transmissionFactor = 10*72/24,
    .degPerFullStep = 1.8,
    .microsteps = 8,
    .endstopInputPullup = true,
    .endstopActiveHigh = true
};

// Axis B
#define PIN_AX_B_STP 33
#define PIN_AX_B_DIR 34
#define PIN_AX_B_CS  1 
#define PIN_AX_B_EN  26
#define PIN_AX_B_ES  6
const AxisConfig axisConfigB = {
    .pinStep = PIN_AX_B_STP,
    .pinDir  = PIN_AX_B_DIR,
    .pinCs   = PIN_AX_B_CS,
    .pinEn   = PIN_AX_B_EN,
    .pinEs   = PIN_AX_B_ES,
    .transmissionFactor = 1*72/24,
    .degPerFullStep = 1.8,
    .microsteps = 8,
    .endstopInputPullup = true,
    .endstopActiveHigh = false
};

// Axis C
#define PIN_AX_C_STP  16
#define PIN_AX_C_DIR  15
#define PIN_AX_C_DIAG 29
#define PIN_AX_C_EN   30
#define PIN_AX_C_ES   9
const AxisConfig axisConfigC = {
    .pinStep = PIN_AX_C_STP,
    .pinDir  = PIN_AX_C_DIR,
    .pinCs   = PIN_AX_C_DIAG,
    .pinEn   = PIN_AX_C_EN,
    .pinEs   = PIN_AX_C_ES
};

// Axis D
#define PIN_AX_D_STP  14
#define PIN_AX_D_DIR  41
#define PIN_AX_D_DIAG 25
#define PIN_AX_D_EN   28
#define PIN_AX_D_ES   4

// Axis R
#define PIN_AX_R_STP  18
#define PIN_AX_R_DIR  17
#define PIN_AX_R_DIAG 31
#define PIN_AX_R_EN   32
#define PIN_AX_R_ES   3

// Axis G
#define PIN_AX_G_PWM   23
#define PIN_AX_G_DIR   22
#define PIN_AX_G_CS    19
#define PIN_AX_G_SLP   21
#define PIN_AX_G_FAULT 20

// Display and touchscreen
#define PIN_TFT_CS    37
#define PIN_TFT_DC    38
#define PIN_TFT_RST   24
#define PIN_TOUCH_CS  36
#define PIN_TOUCH_IRQ 10

// SPI
#define PIN_MOSI 11
#define PIN_MISO 12
#define PIN_SCK  13

#endif