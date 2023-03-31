#include "printer.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "rnp_interface.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

Printer::Printer(uint8_t id,std::string name):
RnpInterface(id,name)
{};

void Printer::setup()
{};

void Printer::update()
{};

void Printer::sendPacket(RnpPacket& data){  
    std::vector<uint8_t> serializedData;
    data.serialize(serializedData);
    
    std::stringstream aout;

    aout<<"Packet:\n";
    aout<<RnpHeader::print(data.header).str()<<"\n";
    for (auto& elem : serializedData){
        aout<<std::hex<<std::to_string((int)elem)<<",";
    }
    aout<<"\n";

    #ifdef ARDUINO
    
    Serial.println(aout.str().c_str());
    #else
    std::cout<<aout.str()<<std::endl;
    #endif

};
