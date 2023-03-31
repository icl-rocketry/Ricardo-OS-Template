#pragma once
/**
 * @file printer.h
 * @author Kiran de Silva
 * @brief Printer Debug Interface -> will just print to std::cout the received packet
 * @version 0.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <memory>
#include <vector>
#include <string>


#include "rnp_interface.h"


class Printer : public RnpInterface{
    public:
        Printer(uint8_t id,std::string name = "printer");

        void setup() override;
        void update() override;

        void sendPacket(RnpPacket& data) override;
        
        const RnpInterfaceInfo* getInfo() override {return &info;};

        ~Printer(){};

    private:
        RnpInterfaceInfo info;
        
};