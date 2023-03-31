#pragma once

#include "rocketcomponent.h"

#include <memory>

// 
#include "Helpers/rrclog.h"

struct RocketActuatorState:public RocketComponentState{
    int32_t currentValue;
};

class RocketActuator: public RocketComponent{
    public:
        RocketActuator(uint8_t id,RocketComponentTypes::TYPES componentType,LIBRRC::RRCLog::LogCallback_t logcb):
        RocketComponent(id,componentType,logcb)
        {};

        virtual void arm() = 0;

        virtual void execute(int32_t param) = 0;

        virtual ~RocketActuator() = 0;

};