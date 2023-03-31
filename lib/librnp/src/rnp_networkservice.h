#pragma once
#include <rnp_networkmanager.h>
#include <memory>
#include <functional>

class RnpNetworkService
{
public:
    /**
     * @brief Construct a new Rnp Network Service object
     *
     * @param ServiceID Identifier of service
     */
    RnpNetworkService(uint8_t ServiceID) : _ServiceID(ServiceID){};

    /**
     * @brief Gets the network callback functor bound to the current instance of the class.
     *
     * @return PacketHandlerCb
     */
    PacketHandlerCb getCallback()
    {
        return [this](packetptr_t packetptr)
        { networkCallback(std::move(packetptr)); };
    };

    uint8_t getServieID() { return _ServiceID; };

    virtual ~RnpNetworkService(){};

private:
    /**
     * @brief Service ID is constant during the duration of the object lifetime by design
     * as the service would need to be un-registed from the netowkr manager before changing its service ID.
     *
     */
    const uint8_t _ServiceID;

    /**
     * @brief Virtual function for the network callback.
     *
     * @param packetptr
     */
    virtual void networkCallback(packetptr_t packetptr) = 0;
};