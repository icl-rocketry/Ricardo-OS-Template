#include "Deploy.h"

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>
#include <libriccore/riccorelogging.h>

#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "system.h"

/**
 * @brief The Deploy state will do nothing except transition to armed when it is
 * armed by another device.
 */
Deploy::Deploy(Types::CoreTypes::SystemStatus_t& systemstatus, Types::CoreTypes::CommandHandler_t& commandhandler, NRCCrosshair& crosshair):
        State(SYSTEM_FLAG::STATE_DEPLOY,systemstatus),
        commandhandler(commandhandler),
        systemstatus(systemstatus),
        crosshair(crosshair) {}

void Deploy::initialize() {
    State::initialize(); // call parent initialize first!

    // Deploy ignition
    crosshair.pyroAdapter.execute(GeneralConfig::PYRO_IGNITION_TIME);
    ignitionTime = millis();
    crosshair.deployed = true;

    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Crosshair deployed at altitude : " + std::to_string(crosshair.baroData.alt));
}

Types::CoreTypes::State_ptr_t Deploy::update() {
    // if (millis() - prevLogMessageTime > 1000) {
    //     RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Deploy heartbeat!");
    //     prevLogMessageTime = millis();
    // }

    if (millis() - ignitionTime < GeneralConfig::PYRO_IGNITION_TIME) {
        // Stay in the ignition state for PYRO_IGNITION_TIME
        return nullptr;
    } else {
        // Else disarm
        crosshair.disarm_base();
    }

    return nullptr;
}

void Deploy::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};