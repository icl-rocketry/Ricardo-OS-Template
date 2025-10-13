#pragma once

#include <librrc/Remote/nrcremoteactuatorbase.h>
#include <librrc/Remote/nrcremotepyro.h>
#include <librrc/Helpers/sensorpoller.h>
#include <librnp/rnp_networkmanager.h>
#include <libriccore/fsm/statemachine.h>
#include <libriccore/systemstatus/systemstatus.h>

#include "Config/Crosshairtypes.h"
#include "Config/systemflags_config.h"
#include "Config/services_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Loggers/TelemetryLogger/telemetrylogframe.h"
#include "Loggers/TelemetryLogger/telemetrylogger.h"

#include "Sensors/DPS368.h"
#include "Sensors/Vrailmonitor.h"
#include "Sensors/sensorStructs.h"

#include <SPI.h>
#include <Wire.h>

#include "States/Idle.h"
#include "States/Deploy.h"
#include "States/Armed.h"
// #include "States/Debug.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"

class NRCCrosshair : public NRCRemoteActuatorBase<NRCCrosshair> {
public:
    NRCCrosshair(RnpNetworkManager& networkmanager, Types::CrosshairTypes_t::SystemStatus_t& systemstate, Types::CoreTypes::CommandHandler_t& commandHandler, Types::CrosshairTypes_t::StateMachine_t& statemachine);

    void setup();
    void update();

    void arm_base(int32_t arg);
    void disarm_base();
    void execute_base(int32_t arg);

    friend class NRCRemoteActuatorBase;
    friend class NRCRemoteBase;

    friend class Idle;
    friend class Armed;
    friend class Deploy;

    RnpNetworkManager& networkmanager;
    Types::CoreTypes::CommandHandler_t& commandHandler;
    Types::CrosshairTypes_t::SystemStatus_t& systemstatus;

    // State

    int ignitionCalls = 0;
    bool deployed = false;

    // Hardware

    // Barometer
    SPIClass spiBaro;
    DPS368 baro;
    SensorStructs::BaroState_t baroData;
    float smoothedBaroAlt;

    // Voltage Rail Monitors
    // VRailMonitor logicRail;
    VRailMonitor qdRail;
    bool lowVoltageTriggered; // Triggered if the rail is low voltage.
    float qdRailVoltage; // Voltage of qd rail (V)

    // Deployment Pyro
    Types::LocalPyro_t pyro;
    Types::LocalPyroAdapter_t pyroAdapter;

    // FSM
    Types::CrosshairTypes_t::StateMachine_t& stateMachine;
    // SPI for SD interface
    SPIClass vspi;

    // SDCard Filestore
    SdFat_Store filestore;

    // Logging
    TelemetryLogframe logFrame;
    TelemetryLogger fileLogger;
    std::string logFilePath;
    bool logToFile = false;

    // I2C for Buck Boost
    TwoWire i2c;
};
