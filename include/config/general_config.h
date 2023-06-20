#pragma once

// //uncomment to show all debug messages
// #define VERBOSE

//nominal charged battery voltage (mV)
// #define full_battery_voltage 4200
// #define warn_battery_voltage 3710 //coresponds to 15%
// #define empty_battery_voltage 3270
namespace GeneralConfig{
//Serial baud rate
static constexpr int SerialBaud = 115200;
//Serial rx buffer size
static constexpr int SerialRxSize = 256;

//I2C frequrency - 4Khz
static constexpr int I2C_FREQUENCY = 400000;
//I2C device addresses
//gps
// static constexpr int I2C_GPS_ADDRESS 0x42
// //gps specific registers
// static constexpr int GPS_NUM_AVAILABLE_BYTES_REGISTER 0xfd
// static constexpr int GPS_DATASTREAM_REGISTER 0xff

// //Lora Specific Config
// #define LORA_SYNC_WORD 0xF3
// //Lora region codes
// //433E6 for Asia
// //866E6 for Europe
// //915E6 for North America
// #define LORA_REGION 866E6
// //define max packet length on any interface in the network
// #define MAX_PACKET_LENGTH 256

// #define ACCEL_SCALE 16
// #define MAG_SCALE 12
// #define GYRO_SCALE 2000

// //ArduinoJson
// #define ARDUINOJSON_ENABLE_COMMENTS 1

};







