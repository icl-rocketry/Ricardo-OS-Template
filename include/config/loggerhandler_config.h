#pragma once

#include <tuple>

#include <libriccore/logging/loggers/coutlogger.h>

namespace RicCoreLoggingConfig
{
    enum class LOGGERS
    {
        SYS, // default system logging
    };

    inline std::tuple logger_list =
        {
            CoutLogger("SYS_LOG")
        };
};