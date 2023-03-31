#pragma once

#include <vector>
#include <memory>

#include <ArduinoJson.h>

#include "controller.h"
#include "controllable.h"
#include "Engine/enginehandler.h"

#include <librrc/configurabledynamichandler.h>

#include "Storage/logController.h"

class ControllerHandler :public ConfigurableDynamicHandler<Controller,ControllerHandler>{
    public:
        ControllerHandler(EngineHandler& enginehandler,LogController& logcontroller):
            ConfigurableDynamicHandler(logcontroller.getLogCB()), 
            _logcontroller(logcontroller),
            _enginehandler(enginehandler)
        {};

        void update(const SensorStructs::state_t& estimator_state);

    protected:

        LogController& _logcontroller; // unsure wether to use log cb or logcontroller, so kept the old way for now

        EngineHandler& _enginehandler; // currently only supporting engines but can be expanded later to support controllable fins or more!

        Controllable* getControllable(JsonObjectConst config);

        friend class ConfigurableDynamicHandler;
        void setupIndividual_impl(size_t id,JsonObjectConst controllerconfig);
        
    
};