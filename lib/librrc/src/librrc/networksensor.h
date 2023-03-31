#pragma once

#include "rocketcomponent.h"
#include "rocketcomponenttype.h"
#include "rocketsensor.h"

#include <rnp_networkmanager.h>

// #include "Storage/logController.h"
#include "Helpers/rrclog.h"
struct NetworkSensorState: public RocketSensorState{
    //no extra members currently
};

class NetworkSensor:public RocketSensor{

    public:
        NetworkSensor(uint8_t id,uint8_t address,uint8_t sourceService,uint8_t destinationService,RnpNetworkManager& networkmanager,LIBRRC::RRCLog::LogCallback_t logcb):
        RocketSensor(id,RocketComponentTypes::TYPES::NET_SENSOR,logcb),
        _address(address),
        _sourceService(sourceService),
        _destinationService(destinationService),
        _networkmanager(networkmanager)
        {
            // logcb("Sensor:" + std::to_string(id) + " constructed" + std::to_string(address));
        };

        void updateState() override;

        /**
         * @brief Network callback function used to process packets recevied by the network manager. This will usually be managed by
         *        a managing object.
         * 
         * @param packetptr 
         */
        void networkCallback(packetptr_t packetptr);

        ~NetworkSensor();

    protected:
        const uint8_t _address;
        const uint8_t _sourceService;
        const uint8_t _destinationService; 

        RnpNetworkManager& _networkmanager;
        NetworkSensorState _state;

         /**
         * @brief Returns a pointer to the state of the component
         * 
         * @return ComponentState* 
         */
        RocketComponentState* p_getState() override {return &_state;};
    
};