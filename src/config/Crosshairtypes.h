#pragma once
#include <cstdint>
#include <memory>

#include "Config/systemflags_config.h"

#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/fsm/state.h>
#include <libriccore/fsm/statemachine.h>
#include <librrc/Remote/nrcremotepyro.h>
#include <librrc/Local/remoteactuatoradapter.h>
#include <Sensors/sensorStructs.h>
#include <librrc/HAL/arduinogpio.h>

template <typename CROSSHAIR_FLAGS_T>
struct CrosshairTypes {
    using SystemStatus_t = SystemStatus<CROSSHAIR_FLAGS_T>;
    using State_t = State<CROSSHAIR_FLAGS_T>;
    using State_ptr_t = std::unique_ptr<State_t>;
    using StateMachine_t = StateMachine<CROSSHAIR_FLAGS_T>;
};

namespace Types {
    using CrosshairTypes_t = CrosshairTypes<SYSTEM_FLAG>;
    using LocalPyro_t = NRCRemotePyro<ArduinoGpio>;
    using LocalPyroAdapter_t = RemoteActuatorAdapter<Types::LocalPyro_t>;
    using LocalPyroMap_t = std::array<LocalPyroAdapter_t *, 1>;
};

class NRCCrosshair;