/*
handles all messages as a bitfield so multiple messages can be contained in a single variable
*/

#pragma once

#include "flags.h"
#include "logController.h"

#include <string>
#include "Helpers/bitwiseflagmanager.h"

class SystemStatus:public BitwiseFlagManager<SYSTEM_FLAG>{
    public:
        
        SystemStatus(LogController& logcontroller);
        void newFlag(SYSTEM_FLAG flag,std::string info);
        void newFlag(SYSTEM_FLAG flag) override ;

        void deleteFlag(SYSTEM_FLAG flag) override;
        void deleteFlag(SYSTEM_FLAG flag,std::string info);

    private:
        LogController& _logcontroller;

};
