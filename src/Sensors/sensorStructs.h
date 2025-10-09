#pragma once
/*
Baro, INA current sense, and ADC sense to help with separation
*/

#include <cstdint>

namespace SensorStructs
{
    struct BaroState_t
    {
        float alt;   // altitude
        float temp;  // temperature
        float press; // pressure
    };

    struct VRailState_t
    {
        float volt;    // Voltage in mV
        int percent; // Percentage in reference to max voltage expeceted
    };

    // struct INA_V_RAIL_t
    // {
    //     int volt;    // Voltage in mV
    //     int current; // Current in mA
    //     int power;   // Power in mW
    //     int percent; // Percentage in reference to max voltage expeceted
    // };

    struct SensorState_t
    {
        // ADC GPIO monitoring
        VRailState_t batV; // GPIO15
        VRailState_t deployV;  // GPIO10

        // Barometer sensor
        BaroState_t baro;

        // INA219 current sensor for pyro
        // INA_V_RAIL_t deprail;
    };
}
