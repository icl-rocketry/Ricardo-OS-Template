#pragma once

#include <stdint.h>
#include <unordered_map>
#include <functional>
#include <initializer_list>

#include <libriccore/commands/commandhandler.h>
#include "rnp_packet.h"

#include "config/forward_decl.h"
#include "commands/commands.h"


namespace Commands
{
    enum class ID : uint8_t
    {
        NoCommand = 0,
        Free_Ram = 250
    };

    inline std::initializer_list<ID> defaultEnabledCommands = {};

    inline std::unordered_map<ID, std::function<void(ForwardDecl_SystemClass &, const RnpPacketSerialized &)>> command_map{
        {ID::Free_Ram, FreeRamCommand}};



}