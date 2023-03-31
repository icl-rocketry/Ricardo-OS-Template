
#include "flight.h"
#include "recovery.h"

#include "flags.h"
#include "stateMachine.h"
#include "ApogeeDetection/apogeedetect.h"


Flight::Flight(stateMachine* sm):
State(sm,SYSTEM_FLAG::STATE_FLIGHT)
// apogeedetect(200,_sm->logcontroller)
{};

void Flight::initialise(){
    State::initialise();
};


State* Flight::update(){

    _sm->enginehandler.update();
    _sm->controllerhandler.update(_sm->estimator.getData());
    _sm->eventhandler.update(_sm->estimator.getData());

    float Ad = _sm->estimator.getData().acceleration(2);

    if (Ad > 0 && !_sm->systemstatus.flagSetOr(SYSTEM_FLAG::FLIGHTPHASE_BOOST)){
        _sm->systemstatus.newFlag(SYSTEM_FLAG::FLIGHTPHASE_BOOST,"Entered Boost Phase");
        _sm->systemstatus.deleteFlag(SYSTEM_FLAG::FLIGHTPHASE_COAST);
    }else if (Ad < 0 && !_sm->systemstatus.flagSetOr(SYSTEM_FLAG::FLIGHTPHASE_COAST)){
        _sm->systemstatus.newFlag(SYSTEM_FLAG::FLIGHTPHASE_COAST,"Entered Coast Phase");
        _sm->systemstatus.deleteFlag(SYSTEM_FLAG::FLIGHTPHASE_BOOST);
    }
    ApogeeInfo apogeeinfo = _sm->apogeedetect.checkApogee(-_sm->estimator.getData().position(2),_sm->estimator.getData().velocity(2),millis());
    if (apogeeinfo.reached){
        _sm->systemstatus.deleteFlag(SYSTEM_FLAG::FLIGHTPHASE_COAST);
        _sm->systemstatus.deleteFlag(SYSTEM_FLAG::FLIGHTPHASE_BOOST);
        _sm->systemstatus.newFlag(SYSTEM_FLAG::FLIGHTPHASE_APOGEE,"Apogee Detected!!");
        _sm->estimator.setApogeeTime(apogeeinfo.time);
        _sm->logcontroller.log("Apogee at " + std::to_string(apogeeinfo.altitude));
        State* recovery_ptr = new Recovery(_sm);
        return recovery_ptr;
    }else{
        return this;
    }
};

void Flight::exitstate(){
    State::exitstate();
};

// bool Flight::apogeeDetect(){ // 20hz

//     if (millis() - prevApogeeDetectTime >= apogeeDelta){
//         prevApogeeDetectTime = millis();
//         altitudeHistory.at(0) = altitudeHistory.at(1);
//         altitudeHistory.at(1) = altitudeHistory.at(2);
//         altitudeHistory.at(2) = _sm->sensors.getData().baro.alt;

//         if ( (altitudeHistory.at(2) < altitudeHistory.at(1)) && (altitudeHistory.at(1) < altitudeHistory.at(0)) && abs(altitudeHistory.at(2) - altitudeHistory.at(0)) > 2){
//             return true;
//         }
//     }


//     return false;
// }