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

#include "States/Idle.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"

#include <Arduino.h>

TwoWire I2C(0);

System::System():
        RicCoreSystem(Commands::command_map,Commands::defaultEnabledCommands,Serial),
        canbus(systemstatus, PinMap::TxCan, PinMap::RxCan, GeneralConfig::CANBUS_ID, "Crosshair"),
        crosshair(networkmanager, systemstatus, commandhandler, statemachine) {};

void System::systemSetup(){

    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);

    // delay(2000);

    //intialize rnp message logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(networkmanager);

    //initialize statemachine with idle state
    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));
    //any other setup goes here

    canbus.setup();

    networkmanager.setNodeType(NODETYPE::HUB);
    networkmanager.setNoRouteAction(NOROUTE_ACTION::BROADCAST,{1,3});
    networkmanager.addInterface(&canbus);

    crosshair.setup();
    networkmanager.registerService(static_cast<uint8_t>(Services::ID::Crosshair), crosshair.getThisNetworkCallback());

    gpio_reset_pin(static_cast<gpio_num_t>(PinMap::CELL_SWITCH));
    pinMode(PinMap::CELL_SWITCH, OUTPUT);
    delay(50);
    digitalWrite(PinMap::CELL_SWITCH, LOW);
}


void System::systemUpdate(){
    crosshair.update();
}