#include "nrcremotemotor.h"
#include "Helpers/rangemap.h"

#include "esp32-hal-ledc.h"

#include <Arduino.h>

void NRCRemoteMotor::setup(){

    ledcSetup(_channel1, freq, timer_width);
    ledcAttachPin(_gpio1, _channel1);

    ledcSetup(_channel2, freq, timer_width);
    ledcAttachPin(_gpio2, _channel2);

}


void NRCRemoteMotor::execute_impl(packetptr_t packetptr){

    SimpleCommandPacket execute_command(*packetptr);
    move_motor(execute_command.arg);
}

void NRCRemoteMotor::move_motor(uint32_t speed)
{
    _value = speed; // update last position of actuator
    if((speed > 100) && (speed <= 200)){    //reverse rotation
        ledcWrite(_channel1, 0);
        ledcWrite(_channel2, LIBRRC::rangemap<uint32_t>(speed, 100, 200, 0, max_duty));
    }
    if((speed > 0) && speed<= 100){    //forward rotation
        ledcWrite(_channel1, LIBRRC::rangemap<uint32_t>(speed, 0, 100, 0, max_duty));
        ledcWrite(_channel2, 0);
    }
    if(speed == 0){   //stop and freewheel
        ledcWrite(_channel1, 0);
        ledcWrite(_channel2, 0);
    }
    
}