#include "engine.h"

#include <ArduinoJson.h>
#include <memory>
#include <rnp_networkmanager.h>

#include <librrc/rocketcomponent.h>


#include "Storage/logController.h"

void Engine::execute(int func){
    switch(func){
        case static_cast<uint8_t>(ENGINE_EXECUTE::IGNITE):
        {
            ignite();
            break;
        }
        case static_cast<uint8_t>(ENGINE_EXECUTE::SHUTDOWN):
        {
            shutdown();
            break;
        }
        default:
        {
            break;
        }
    }
};

void Engine::ignite(){
    log("Ignition Called!");
    getStatePtr()->ignitionTime = millis();

};

void Engine::shutdown(){
    log("Shutdown Called!");
    getStatePtr()->shutdownTime = millis();
};

void Engine::log(const std::string message){
    _logcontroller.log("Engine:" + std::to_string(getID()) + message);
}

Engine::~Engine(){};