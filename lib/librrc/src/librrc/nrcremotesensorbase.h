#pragma once

#include "nrcremotebase.h"
#include <rnp_networkmanager.h>
#include "rocketcomponent.h"

template <typename Derived>

class NRCRemoteSensorBase : public NRCRemoteBase<Derived>
{
    public:
        NRCRemoteSensorBase(RnpNetworkManager& networkmanager) : NRCRemoteBase<Derived>(networkmanager)
        {
            this->_state.newFlag(COMPONENT_STATUS_FLAGS::NOMINAL); // initlaize sensor to nominal state
        };

    protected:
        void updateSensorValue(float value)
        {
            this->_value = *reinterpret_cast<int32_t*>(&value); // convert float to int32_t bytes
        }
};