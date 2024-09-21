#include "system.h"

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


System::System():
RicCoreSystem(Commands::command_map,Commands::defaultEnabledCommands,Serial),
vspi(0),
filestore(vspi,PinMap::sdcs,SD_SCK_MHZ(50),false,&systemstatus)
{};


void System::systemSetup(){
    
    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);

    delay(2000);

    vspi.begin(PinMap::V_SCLK,PinMap::V_MISO,PinMap::V_MOSI);
    vspi.setFrequency(SD_SCK_MHZ(20));

    pinMode(PinMap::sdcs,OUTPUT);
    digitalWrite(PinMap::sdcs,HIGH);
   
    //intialize rnp message logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(networkmanager);

    //initialize statemachine with idle state
    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));
    //any other setup goes here
    
    
};


void System::systemUpdate(){};