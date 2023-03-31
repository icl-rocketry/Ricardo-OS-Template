/**
 * @file NRCRemoteBase.h
 * @author Kiran de Silva
 * @brief Base class for the Networked Rocket Component (NRC) handler. Defines the basic required
 * command processing for NRC type components. Uses CRTP to reduce runtime polymorphism overhead 
 * as it is not expected to cast to parent class ever. SHould be straightforward to change in 
 * the future or simply provide a runtime polymorphic derived class from this. Extended command handler 
 * implemented if a more complex componet is required.
 * @version 0.1
 * @date 2022-07-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <functional>

#include "rocketcomponent.h"

#include "packets/nrcpackets.h"
#include <default_packets/simplecommandpacket.h>

#include <rnp_networkmanager.h>

template<class Derived>
class NRCRemoteBase
{
public:
    NRCRemoteBase(RnpNetworkManager& networkmanager):
    _networkmanager(networkmanager)
    {};
    /*
     * @brief Returns network callback bound to instance
     *
     * @return std::function<void(packetptr_t)>
     */
    std::function<void(packetptr_t)> getThisNetworkCallback()
    {
        return [this](packetptr_t packetptr)
        { handlecommand(std::move(packetptr)); };
    };


    const component_status_flags_t getStatus(){return _state.getStatus();};
    const int32_t getValue(){return _value;};

protected:

    RnpNetworkManager& _networkmanager;
    LIBRRC::BitwiseFlagManager<COMPONENT_STATUS_FLAGS,component_status_flags_t> _state;
    int32_t _value;

    void handlecommand(packetptr_t packetptr){
        //check packet is a command packet
        if (packetptr->header.type == static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND)){
            
            const NRCPacket::NRC_COMMAND_ID commandID = static_cast<NRCPacket::NRC_COMMAND_ID>(CommandPacket::getCommand(*packetptr));
           
            switch(commandID){
                case NRCPacket::NRC_COMMAND_ID::GETSTATE:
                {
                    static_cast<Derived*>(this)->getstate_impl(std::move(packetptr));
                    break;
                }
                default:
                {
                    static_cast<Derived*>(this)->extendedCommandHandler_impl(commandID,std::move(packetptr));
                    break;
                }
            }

        }
    }
    //default implementations - can be overriden by function hiding as this is crtp
    /**
     * @brief Handle getstate requests
     * 
     * @param packetptr 
     */
    void getstate_impl(packetptr_t packetptr)
    {
        SimpleCommandPacket getstate_request(*packetptr);
        NRCStatePacket getstate_response;
        RnpHeader::generateResponseHeader(getstate_request.header,getstate_response.header);
        getstate_response.state = _state.getStatus();
        getstate_response.value = _value;
        _networkmanager.sendPacket(getstate_response);

        

    };
   
    void extendedCommandHandler_impl(const NRCPacket::NRC_COMMAND_ID commandID,packetptr_t packetptr){};
     
};