#pragma once

// ESP32-S3 DevKitC-1 N16R8 pin map. Full wiring: docs/wiring.md
// Keep away from: GPIO 35-37 (used by the 8MB PSRAM), 0/3/45/46 (boot pins),
// 19/20 (native USB), 43/44 (COM port), 26-32 (flash).

// Display GC9A01 1.28" round, SPI
constexpr int PIN_TFT_SCLK = 12;  // board label "SCL" (SPI clock, not I2C)
constexpr int PIN_TFT_MOSI = 11;  // board label "SDA" (SPI data, not I2C)
constexpr int PIN_TFT_CS   = 10;
constexpr int PIN_TFT_DC   = 9;
constexpr int PIN_TFT_RST  = 14;

// Touch sensor TTP223 (HIGH while touched)
constexpr int PIN_TOUCH = 8;

// Mic INMP441, I2S in (step 2)
constexpr int PIN_MIC_SCK = 4;
constexpr int PIN_MIC_WS  = 5;
constexpr int PIN_MIC_SD  = 6;

// Amp MAX98357A, I2S out (step 2)
constexpr int PIN_AMP_BCLK = 15;
constexpr int PIN_AMP_LRC  = 16;
constexpr int PIN_AMP_DIN  = 7;

// Servos SG90 (step 3)
constexpr int PIN_SERVO_PAN  = 17;
constexpr int PIN_SERVO_TILT = 18;
