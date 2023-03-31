#pragma once
#include "nrcremotebase.h"
#include <rnp_networkmanager.h>
#include "rocketcomponent.h"

#include <Arduino.h>

template <typename Derived>
class NRCRemoteActuatorBase : public NRCRemoteBase<Derived>
{
public:
    NRCRemoteActuatorBase(RnpNetworkManager &networkmanager) : NRCRemoteBase<Derived>(networkmanager)
    {
        this->_state.newFlag(COMPONENT_STATUS_FLAGS::DISARMED);//initalize disarmed
    };

protected:
    friend class NRCRemoteBase<Derived>;

    void extendedCommandHandler_impl(const NRCPacket::NRC_COMMAND_ID commandID,packetptr_t packetptr)
    {
        switch (commandID)
        {
        case NRCPacket::NRC_COMMAND_ID::EXECUTE:
        {
            if (this->_state.flagSet(COMPONENT_STATUS_FLAGS::NOMINAL))
            {
                static_cast<Derived *>(this)->execute_impl(std::move(packetptr));
            }
            break;
        }
        case NRCPacket::NRC_COMMAND_ID::ARM:
        {
            static_cast<Derived *>(this)->arm_impl(std::move(packetptr));
            break;
        }
        case NRCPacket::NRC_COMMAND_ID::DISARM:
        {
            static_cast<Derived *>(this)->disarm_impl(std::move(packetptr));
            break;
        }
        default:
        {
            break;
        }
        }
    }

    /**
     * @brief Arm the component, default implementation simply changes state
     * 
     * @param packetptr 
     */
    void arm_impl(packetptr_t packetptr){
        // check for any other errors, only arm if we are error free

        if (this->_state.getStatus() == static_cast<uint16_t>(COMPONENT_STATUS_FLAGS::DISARMED)){
            this->_state.deleteFlag(COMPONENT_STATUS_FLAGS::DISARMED);
            this->_state.newFlag(COMPONENT_STATUS_FLAGS::NOMINAL);

        }
        
    };
    /**
     * @brief Disarm component
     * 
     * @param packetptr 
     */
    void disarm_impl(packetptr_t packetptr){
        this->_state.deleteFlag(COMPONENT_STATUS_FLAGS::NOMINAL);
        this->_state.newFlag(COMPONENT_STATUS_FLAGS::DISARMED);
    };
};