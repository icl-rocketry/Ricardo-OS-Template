#pragma once


namespace GeneralConfig{
    //Serial baud rate
    static constexpr int SerialBaud = 115200;
    //Serial rx buffer size
    static constexpr int SerialRxSize = 256;

    //I2C frequrency - 4Khz
    static constexpr int I2C_FREQUENCY = 400000;

    // Vrail monitor voltage divider resistors
    // static constexpr int LOGIC_r1 = 10;
    // static constexpr int LOGIC_r2 = 1.8;
    // static constexpr int LOGIC_MAX_V = 0;
    // static constexpr int LOGIC_LOW_V = 0;
    // static constexpr int LOGIC_MIN_V = 0;

    static constexpr float DEPLOYMENT_ALTITUDE = 650;
    static constexpr float DEPLOYMENT_DALT = 50;

    static constexpr int DEPLOY_r1 = 10;
    static constexpr int DEPLOY_r2 = 18;
    static constexpr int DEPLOY_MAX_V = 20;
    static constexpr int DEPLOY_LOW_V = 5;
    static constexpr int DEPLOY_MIN_V = 0;

    static constexpr int CANBUS_ID = 3;

    static constexpr unsigned long PYRO_IGNITION_TIME = 5000;

    static constexpr const char* LogFilename = "log";

    static constexpr unsigned long SD_LOG_INTERVAL = 100;

    static constexpr int BUCK_BOOST_I2C_ADDR = 0x74;
};







