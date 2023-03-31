/*
State during flight portion of rocket controlling thrust output and relaying telemetry to ground. Limited commands avaliable
*/


#pragma once

#include "state.h"


#include <array>
#include "ApogeeDetection/apogeedetect.h"



class Flight: public State {
  public:
    Flight(stateMachine* sm);
    void initialise();
    State* update();
    void exitstate();
  private:
    // std::array<float,3> altitudeHistory;
    // bool apogeeDetect();
    // uint32_t prevApogeeDetectTime;
    // const uint16_t apogeeDelta;
    // ApogeeDetect apogeedetect;
};


