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
        State(SYSTEM_FLAG::STATE_ARMED,systemstatus),
        systemstatus(systemstatus),
        commandhandler(commandhandler),
        armIdleTime(3000),
        crosshair(crosshair) {};

void Armed::initialize() {
    State::initialize(); // call parent initialize first!

    armStartTime = millis();
};

Types::CoreTypes::State_ptr_t Armed::update() {
    // if (millis() - prevLogMessageTime > 1000) {
    //     RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Armed heartbeat! Alt : " + std::to_string(crosshair.smoothedBaroAlt) + ", prs : " + std::to_string(crosshair.baroData.press) + ", rbus voltage : " + std::to_string(crosshair.qdRailVoltage) + ", lowvt : " + std::string(crosshair.lowVoltageTriggered ? "true" : "false") + ", overdep : " + std::string(hasGoneOverDeploymentAlt ? "true" : "false"));
    //     prevLogMessageTime = millis();
    // }

    // Dont do anything for until timeout has run out
    if (millis() - armStartTime < armIdleTime) {
        return nullptr;
    }

    if (crosshair.smoothedBaroAlt > deploymentAlt + dAlt) {
        hasGoneOverDeploymentAlt = true;
    }

    // Test conditions for deployment
    if (hasGoneOverDeploymentAlt && (crosshair.smoothedBaroAlt < deploymentAlt - dAlt) && crosshair.lowVoltageTriggered) {
        return std::make_unique<Deploy>(systemstatus, commandhandler, crosshair);
    }

    return nullptr;
};

void Armed::exit() {
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};