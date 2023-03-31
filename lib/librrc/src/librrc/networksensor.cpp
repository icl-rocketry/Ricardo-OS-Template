#include "networksensor.h"
#include "packets/nrcpackets.h"
#include <default_packets/simplecommandpacket.h>
#include <Arduino.h> // needed for millis

// #include "Storage/logController.h"

NetworkSensor::~NetworkSensor(){};

void NetworkSensor::networkCallback(packetptr_t packetptr){
    if (packetptr->header.source != _address){
        return; // packet had incorrect data
    }
    switch (packetptr->header.type){
        case(static_cast<uint8_t>(NRCPacket::TYPES::NRC_STATE)):
        {
            NRCStatePacket nrcstatepacket(*packetptr); //deserialize state packet
            _state.lastNewStateUpdateTime = millis();
            _state.sensorValue = *reinterpret_cast<float*>(&nrcstatepacket.value); // reinterpret int32 as a float
            _state.trackRemoteStatus(nrcstatepacket.state);
        }
    
    }

   
}

void NetworkSensor::updateState(){
    
    SimpleCommandPacket getstate_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::GETSTATE),0); 
    getstate_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    getstate_packet.header.source = _networkmanager.getAddress();
    getstate_packet.header.source_service = _sourceService;
    getstate_packet.header.destination = _address;
    getstate_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(getstate_packet);
    
    _state.lastNewStateRequestTime = millis();
}