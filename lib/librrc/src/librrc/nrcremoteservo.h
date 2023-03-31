#pragma once

#include "nrcremoteactuatorbase.h"

#include <rnp_networkmanager.h>
#include <rnp_packet.h>

class NRCRemoteServo : public NRCRemoteActuatorBase<NRCRemoteServo>
{

public:
    NRCRemoteServo(uint8_t gpio,
                    uint8_t channel,
                    RnpNetworkManager &networkmanager,
                    uint16_t default_angle = 0,
                    uint16_t min_angle = 0,
                    uint16_t max_angle = 180,
                    uint16_t min_counts = counts(500),
                    uint16_t max_counts = counts(2400)
                    ): 
    NRCRemoteActuatorBase(networkmanager),
        _gpio(gpio),
        _channel(channel),
        _default_angle(default_angle),
        _min_angle(min_angle),
        _max_angle(max_angle),
        _min_counts(min_counts),
        _max_counts(max_counts)
        {};

    void setup();

    /**
     * @brief Temporary implementation of reset. Drives servo to min angle position. This needs to be updated
     * to be configurable.
     * 
     */
    void reset();
   
protected:
    friend class NRCRemoteActuatorBase;



    const uint8_t _gpio;
    const uint8_t _channel;
    const uint16_t _default_angle;
    const uint16_t _min_angle;
    const uint16_t _max_angle;
    const uint16_t _min_counts;
    const uint16_t _max_counts;


    void execute_impl(packetptr_t packetptr);

    uint16_t angletocounts(uint16_t angle);

    static constexpr int timer_width = 16;
    static constexpr int freq = 50;
    
   static constexpr int counts(int usec){
       return (int)(float(usec) / (float(1000000/freq)/float(std::pow(2,timer_width))));
       
    }
};