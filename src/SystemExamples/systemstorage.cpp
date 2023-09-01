#include "systemstorage.h"

#include <memory>
#include <string>

#include <libriccore/riccoresystem.h>

#include <HardwareSerial.h>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Commands/commands.h"
#include <librnp/default_packets/simplecommandpacket.h>

#include "States/idle.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"


SystemStorage::SystemStorage():
RicCoreSystem(Commands::command_map,Commands::defaultEnabledCommands,Serial),
vspi(0),
filestore(vspi,PinMap::sdcs,SD_SCK_MHZ(50),false,&systemstatus),
data(200,120)
{};


void SystemStorage::systemSetup(){
    
    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);

    delay(2000);

    vspi.begin(PinMap::V_SCLK,PinMap::V_MISO,PinMap::V_MOSI);
    vspi.setFrequency(SD_SCK_MHZ(50));

    pinMode(PinMap::sdcs,OUTPUT);
    digitalWrite(PinMap::sdcs,HIGH);
   
    //intialize rnp message logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(networkmanager);

    //initialize statemachine with idle state
    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));
    //any other setup goes here
    filestore.setup();
    std::vector<directory_element_t> files;
    bool error = filestore.ls(files);
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(std::to_string(error));
    for (auto e : files)
    {
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(e.name);
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(std::to_string(e.size));
     
    }

    file_ptr = filestore.open("test.txt",(FILE_MODE)((uint8_t)FILE_MODE::WRITE | (uint8_t)FILE_MODE::CREATE | (uint8_t)FILE_MODE::AT_END));

    
    
};


void SystemStorage::systemUpdate(){
    uint32_t curr_time = micros();
    if (curr_time - prevtime > 1000)
    {
        std::string time_string = "\n" + std::to_string(curr_time) + "\n" + std::to_string(curr_time - prevtime) + "\n";
        if (file_ptr != nullptr)
        {
            file_ptr->appendCopy(data);
            file_ptr->appendCopy(std::vector<uint8_t>(time_string.begin(),time_string.end()));
        }
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(std::to_string((uint8_t)filestore.getState()));
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(std::to_string(esp_get_free_heap_size()));

        prevtime = curr_time;
        
     }


};