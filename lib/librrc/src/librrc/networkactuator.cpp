#include <Arduino.h> // needed for millis()


#include "networkactuator.h"
#include <default_packets/simplecommandpacket.h>

#include "packets/nrcpackets.h"





void NetworkActuator::execute(int32_t param){
    

    SimpleCommandPacket execute_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::EXECUTE),static_cast<uint32_t>(param)); 
    execute_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    execute_packet.header.source = _networkmanager.getAddress();
    execute_packet.header.source_service = _sourceService;
    execute_packet.header.destination = _address;
    execute_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(execute_packet);

};

void NetworkActuator::updateState(){
    SimpleCommandPacket getstate_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::GETSTATE),0); 
    getstate_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    getstate_packet.header.source = _networkmanager.getAddress();
    getstate_packet.header.source_service = _sourceService;
    getstate_packet.header.destination = _address;
    getstate_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(getstate_packet);
    _state.lastNewStateRequestTime = millis();
};

void NetworkActuator::networkCallback(packetptr_t packetptr){
    if (packetptr->header.source != _address){
        #ifdef _RICDEBUG
        std::cout<<"bad packet source"<<std::endl;
        #endif
        return; // packet had incorrect data
    }
    switch (packetptr->header.type){
        case(static_cast<uint8_t>(NRCPacket::TYPES::NRC_STATE)):
        {
            NRCStatePacket nrcstatepacket(*packetptr); //deserialize state packet
            _state.lastNewStateUpdateTime = millis();
            _state.currentValue = nrcstatepacket.value;
            _state.trackRemoteStatus(nrcstatepacket.state);
        }
    
    }

   
};

void NetworkActuator::arm(){
    SimpleCommandPacket arm_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::ARM),0);  // future implementation can use a magic value here effectivley authenitcating the arm command?
    arm_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    arm_packet.header.source = _networkmanager.getAddress();
    arm_packet.header.source_service = _sourceService;
    arm_packet.header.destination = _address;
    arm_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(arm_packet);
}


