#include "Armed.h"

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>
#include <libriccore/riccorelogging.h>

#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "system.h"

/**
 * @brief The Armed state will evaluate state changes based on data read in
 * from main crosshair module.
 *
 * It will transition to deploy state if needed.
 */
Armed::Armed(Types::CoreTypes::SystemStatus_t& systemstatus, Types::CoreTypes::CommandHandler_t& commandhandler, NRCCrosshair& crosshair):
        State(SYSTEM_FLAG::STATE_IDLE,systemstatus),
        systemstatus(systemstatus),
        commandhandler(commandhandler),
        crosshair(crosshair) {};

void Armed::initialize() {
    State::initialize(); // call parent initialize first!
};

Types::CoreTypes::State_ptr_t Armed::update() {
    if (millis() - prevLogMessageTime > 1000) {
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Armed heartbeat!");
        prevLogMessageTime = millis();
    }

    if (crosshair.baroData.alt > deploymentAlt + dAlt) {
        hasGoneOverDeploymentAlt = true;
    }

    // Test conditions for deployment
    if (hasGoneOverDeploymentAlt && (crosshair.baroData.alt - dAlt < deploymentAlt) && crosshair.lowVoltageTriggered) {
        return std::make_unique<Deploy>(systemstatus, commandhandler, crosshair);
    }

    return nullptr;
};

void Armed::exit() {
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};