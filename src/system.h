#pragma once

#include <libriccore/riccoresystem.h>
#include <libriccore/networkinterfaces/can/canbus.h>

#include <memory>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"

#include "nrccrosshair.h"

#include "Commands/commands.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"

#include <SPI.h>

class System : public RicCoreSystem<System,SYSTEM_FLAG, Commands::ID>
{
public:

    System();

    void systemSetup();

    void systemUpdate();

private:
    // Can bus interface
    CanBus<SYSTEM_FLAG> canbus;

public:
    // Board controller
    NRCCrosshair crosshair;
};