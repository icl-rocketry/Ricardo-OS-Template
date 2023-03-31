#pragma once

#include "nrcremoteactuatorbase.h"

#include "networksensor.h"

#include <rnp_networkmanager.h>
#include <rnp_packet.h>

class NRCRemoteTempSSR : public NRCRemoteActuatorBase<NRCRemoteTempSSR>
{

public:
    NRCRemoteTempSSR(uint8_t gpio,RnpNetworkManager &networkmanager): 
        NRCRemoteActuatorBase(networkmanager),
        _gpio(gpio),
        _netTemp1(nullptr),
        _netTemp2(nullptr)
        {};
    
    NRCRemoteTempSSR(uint8_t gpio,RnpNetworkManager &networkmanager,NetworkSensor* netTemp1): 
        NRCRemoteActuatorBase(networkmanager),
        _gpio(gpio),
        _netTemp1(netTemp1),
        _netTemp2(nullptr)
        {};
    
    NRCRemoteTempSSR(uint8_t gpio,RnpNetworkManager &networkmanager,NetworkSensor* netTemp1,NetworkSensor* netTemp2): 
        NRCRemoteActuatorBase(networkmanager),
        _gpio(gpio),
        _netTemp1(netTemp1),
        _netTemp2(netTemp2)
        {};


    void setup();

    /**
     * @brief Update temperature using networked temperature sensors. If none have been provided the ssr will turn off and ERROR state will be raised.
     * Maybe a discrete error state is required 
     * 
     */
    void update();

    /**
     * @brief Update method overload allowing using a locally read thermistor or using a data source from
     *  a non nrc type data source
     * 
     * @param sensor_temperature in deg c
     */
    void update(float sensor_temperature);
   
protected:
    friend class NRCRemoteActuatorBase;

    const uint8_t _gpio;

    void execute_impl(packetptr_t packetptr);

private:
    NetworkSensor* _netTemp1;
    NetworkSensor* _netTemp2; // optional

};