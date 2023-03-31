#include "systemstatus.h"
#include "flags.h"
#include "stateMachine.h"
#include "logController.h"

#include <string>


SystemStatus::SystemStatus(LogController& logcontroller):
BitwiseFlagManager::BitwiseFlagManager(0),
_logcontroller(logcontroller)
{};

void SystemStatus::newFlag(SYSTEM_FLAG flag,std::string info){
    BitwiseFlagManager::newFlag(flag);
    //_sm->logcontroller.log(_status,static_cast<uint32_t>(flag),info);
    _logcontroller.log(static_cast<uint32_t>(getStatus()),static_cast<uint32_t>(flag),info);

};


void SystemStatus::newFlag(SYSTEM_FLAG flag){
     BitwiseFlagManager::newFlag(flag);
    //_sm->logcontroller.log(_status,static_cast<uint32_t>(flag),"flag raised");
    _logcontroller.log(static_cast<uint32_t>(getStatus()),static_cast<uint32_t>(flag),"flag raised");
};


void SystemStatus::deleteFlag(SYSTEM_FLAG flag){
    BitwiseFlagManager::deleteFlag(flag);
   // _sm->logcontroller.log(_status,static_cast<uint32_t>(flag),"flag removed");
    _logcontroller.log(static_cast<uint32_t>(getStatus()),static_cast<uint32_t>(flag),"flag removed");
};

void SystemStatus::deleteFlag(SYSTEM_FLAG flag,std::string info){

    BitwiseFlagManager::deleteFlag(flag);
   //_sm->logcontroller.log(_status,static_cast<uint32_t>(flag),info);
    _logcontroller.log(static_cast<uint32_t>(getStatus()),static_cast<uint32_t>(flag),info);
};


