/*
**********************
* PINS               *
**********************
 */
#pragma once

namespace PinMap{
    // Barometer SPI Pins
    static constexpr int BARO_MISO = 14;
    static constexpr int BARO_MOSI = 13;
    static constexpr int BARO_SCLK = 11;
    static constexpr int BARO_CS = 12;

    // Sd card
    static constexpr int SD_SCLK = 48;
    static constexpr int SD_MISO = 47;
    static constexpr int SD_MOSI = 2;
    static constexpr int SD_CS = 5;

    // Can
    static constexpr int TxCan = 18;
    static constexpr int RxCan = 17;

    // Pyro
    static constexpr int PyroNuke = 38;
    static constexpr int PyroCont = 39;

    // ADC VRail monitor pins
    // static constexpr int BatteryV = 15;
    static constexpr int QDV = 10;

    static constexpr int CELL_SWITCH = 42;

    // I2C
    static constexpr int I2C_SCL = 35;
    static constexpr int I2C_SDA = 36;
};


