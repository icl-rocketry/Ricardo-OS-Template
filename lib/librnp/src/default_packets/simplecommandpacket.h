#pragma once
#include "rnp_packet.h"
#include "rnp_serializer.h"


#include <vector>

using command_t = uint8_t; // define the type of command id type

namespace CommandPacket{
    command_t getCommand(const RnpPacketSerialized& packet); // extracts command id from a command serivce packet
};


class SimpleCommandPacket: public RnpPacket{
    private:
        static constexpr auto getSerializer()
        {
            auto ret = RnpSerializer(
                &SimpleCommandPacket::command,
                &SimpleCommandPacket::arg
            );
            return ret;
        }
    public:
        ~SimpleCommandPacket();
        SimpleCommandPacket(uint8_t command, uint32_t arg);

        /**
         * @brief Deserialize Command Packet from serialized data
         * 
         * @param packet 
         */
        SimpleCommandPacket(const RnpPacketSerialized& packet);

        /**
         * @brief Serialize into provided buffer
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override;

        //data members
        command_t command;

        uint32_t arg;


        static constexpr size_t size(){
            return getSerializer().member_size();
        }
};