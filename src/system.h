#pragma once

#include <libriccore/riccoresystem.h>

#include "config/systemflags_config.h"
#include "config/commands_config.h"
#include "config/pinmap_config.h"

#include "commands/commands.h"

class System : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        System():
        RicCoreSystem(Commands::command_map,Commands::defaultEnabledCommands,)
        {};

        void systemSetup();

        void systemUpdate(){};


}