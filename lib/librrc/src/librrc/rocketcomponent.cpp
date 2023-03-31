#include "rocketcomponent.h"

#include <string>
#include <Arduino.h>

RocketComponent::~RocketComponent(){};

//Returns true if there is an error !!
bool RocketComponent::flightCheck(uint32_t networkRetryInterval,uint32_t stateExpiry,std::string handler){
    const RocketComponentState* currentState = this->getState();
    if (currentState == nullptr){
        #ifdef _RICDEBUG
        throw std::runtime_error("flightCheck() called on base class instance without a derived class!");
        #endif
        this->_logcb("flightCheck() called on base class instance without a derived class!");
        return 1; 
    }

    const uint8_t cid = this->getID();

    if (currentState->lastNewStateRequestTime >= currentState->lastNewStateUpdateTime) //component hasnt sent new update
    {
        if (millis() - currentState->lastNewStateRequestTime > networkRetryInterval) //component timeout
        { //maybe packet got lost on the network? might indicate something more serious however
            this->_logcb(handler + " Component: " + std::to_string(cid) + " not responding!");
            //update state of component to no response error
            this->p_getState()->newFlag(COMPONENT_STATUS_FLAGS::ERROR_NORESPONSE);
            // try requesting an update again
            this->updateState(); 
            return 1;
        }
    }else if (millis()-currentState->lastNewStateRequestTime>stateExpiry)
    {
        //current state has expired, request new state update
       
        this->updateState();
    }
    
    if (!this->p_getState()->flagSet(COMPONENT_STATUS_FLAGS::NOMINAL))
    {
        //check if the component state has changedf
        if (currentState->getStatus() != currentState->previousStatus)
        {
            //log the changes
            this->_logcb(handler + " Component: " + std::to_string(cid) + " error: " + std::to_string(currentState->getStatus()));
            //update previous state
            this->p_getState()->previousStatus = currentState->getStatus();
        }
        return 1;
    }

    return 0;
}

