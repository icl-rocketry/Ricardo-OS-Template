#pragma once

#include <iostream>
#include <unistd.h>
#include <string>

#include "Helpers/bitwiseflagmanager.h"
#include "Helpers/rrclog.h"

#include "rocketcomponenttype.h"




enum class COMPONENT_STATUS_FLAGS:uint16_t{
    NOMINAL = (1<<0),
    DISARMED = (1<<1),
    ERROR_NORESPONSE = (1<<2),
    ERROR_CONTINUITY = (1<<3),
    ERROR_PINS = (1<<4),
    ERROR_I2C = (1<<5),
    ERROR = (1<<6) //generic error
};

using component_status_flags_t = typename std::underlying_type<COMPONENT_STATUS_FLAGS>::type;
// using system_flag_t = std::underlying_type<SYSTEM_FLAG>::type;

struct RocketComponentState : public LIBRRC::BitwiseFlagManager<COMPONENT_STATUS_FLAGS>
{

    uint32_t lastNewStateUpdateTime;
    uint32_t lastNewStateRequestTime;
    component_status_flags_t previousStatus; // used to track changes in state

    /**
     * @brief Allows tracking of the state of remote components such as
     * networked components
     * 
     * @param remoteState 
     */
    void trackRemoteStatus(uint16_t remoteState)
    {
        _statusString = remoteState;
    }
};

class RocketComponent{
    public:
        RocketComponent(uint8_t id,RocketComponentTypes::TYPES componentType,LIBRRC::RRCLog::LogCallback_t logcb):
        _id(id),
        _componentType(componentType),
        _logcb(logcb)
        {};
        
        /**
         * @brief Returns a const pointer to the component state for external objects to 
         * get the last received state of the component.
         * 
         * @return const RocketComponentState* 
         */
        const RocketComponentState* getState(){return p_getState();};
        virtual void updateState() = 0;
        virtual bool flightCheck(uint32_t netRetryInterval,uint32_t stateExpiry,std::string handler);
        virtual ~RocketComponent() = 0;

        uint8_t getID(){return _id;};
        RocketComponentTypes::TYPES getComponentType(){return _componentType;};

    protected:
        const uint8_t _id;
        const RocketComponentTypes::TYPES _componentType;

        /**
         * @brief Returns a non const pointer to the component state allowing parent class
         * to update state of component
         * 
         * @return RocketComponentState* 
         */
        virtual RocketComponentState* p_getState() = 0;


        LIBRRC::RRCLog::LogCallback_t _logcb;
            

};

