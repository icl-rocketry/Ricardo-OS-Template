#pragma once
#include "rnp_networkmanager.h"
#include <string>

#if (defined ESP32 && defined ARDUINO)
#include "Preferences.h"

namespace RnpNvsSave
{   

    static bool SaveToNVS(RnpNetworkManagerConfig const & config){
        Preferences pref;
        if (!pref.begin("Rnp_Config")){
            return true;
        }
          
        int error = 0;

        pref.putUChar("address",config.currentAddress) ? : error++;
        pref.putUChar("nodeType",static_cast<uint8_t>(config.nodeType)) ? : error++;
        pref.putUChar("noRouteAction",static_cast<uint8_t>(config.noRouteAction)) ? : error++;
        pref.putBool("routeGen",config.routeGenEnabled) ? : error++;

        return error;  
    };

    static bool ReadFromNVS(RnpNetworkManagerConfig& config){
        Preferences pref;
        if (!pref.begin("Rnp_Config")){
            return true;
        }
          
        config.currentAddress = pref.getUChar("address",0);
        config.nodeType = static_cast<NODETYPE>(pref.getUChar("nodeType"));
        config.noRouteAction = static_cast<NOROUTE_ACTION>(pref.getUChar("noRouteAction"));
        config.routeGenEnabled = pref.getBool("routeGen",config.routeGenEnabled);

        //at the current time we can only really tell if the addres field was read back incorrectly as 0 is an illegal address
        //however when we rewrite prefrences we can return a status out of the get methods to catch if there was an error
        if (config.currentAddress == 0){
            return true;
        }
        return false;  
    };
};

#else
#include <iostream>
namespace RnpNvsSave
{
    static bool SaveToNVS(RnpNetworkManagerConfig const & config){
        std::cout<<"Address:" + std::to_string(config.currentAddress)<<"\n";
        std::cout<<"nodeType:" + std::to_string(static_cast<uint8_t>(config.nodeType))<<"\n";
        std::cout<<"noRouteAction:" + std::to_string(static_cast<uint8_t>(config.noRouteAction))<<"\n";
        std::cout<<"routeGenEnabled:" + std::to_string(config.routeGenEnabled)<<"\n";
        return true;
    };

    static bool ReadFromNVS(RnpNetworkManagerConfig& config){
        //fake data for testing and nvs doesnt exist on non esp32 platforms anyway
        config.currentAddress = 10;
        config.nodeType = static_cast<NODETYPE>(1);
        config.noRouteAction = static_cast<NOROUTE_ACTION>(1);
        config.routeGenEnabled = 1;
        return true;
    }
};


#endif