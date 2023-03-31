#pragma once

#include "nrcremoteactuatorbase.h"

#include <rnp_networkmanager.h>
#include <rnp_packet.h>
#include <esp_task.h>

#include <functional>

class NRCRemotePyro : public NRCRemoteActuatorBase<NRCRemotePyro>
{

public:
    NRCRemotePyro(uint8_t firePin,uint8_t contPin,RnpNetworkManager &networkmanager) : 
    NRCRemoteActuatorBase(networkmanager),
    _firePin(firePin),
    _contPin(contPin)
    {};

    void setup();

protected:
    friend class NRCRemoteActuatorBase;
    friend class NRCRemoteBase;
    const uint8_t _firePin;
    const uint8_t _contPin;

    TaskHandle_t async_off_task_handle = nullptr;

    void execute_impl(packetptr_t packetptr);
    void arm_impl(packetptr_t packetptr);
    void getstate_impl(packetptr_t packetptr);

    void updateContinuity();

};