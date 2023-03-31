#pragma once

#include <rnp_packet.h>
#include <rnp_serializer.h>

#include <vector>

//(N)etwork(R)ocket(C)omponent

namespace NRCPacket{
    enum class TYPES:uint8_t{
        NRC_COMMAND = 0,
        NRC_STATE = 1
    };

    enum class NRC_COMMAND_ID:uint8_t{
        NOCOMMAND=0,
        GETSTATE=1,
        EXECUTE=2,
        ARM=3,
        DISARM=4
    };

};



class NRCStatePacket : public RnpPacket{
    private:
    //serializer framework
        static constexpr auto getSerializer()
        {
            auto ret = RnpSerializer(
                &NRCStatePacket::state,
                &NRCStatePacket::value
            );
            return ret;
        }
        
    public:
        ~NRCStatePacket();

        NRCStatePacket();
        /**
         * @brief Deserialize NRCstate Packet
         * 
         * @param data 
         */
        NRCStatePacket(const RnpPacketSerialized& packet);

        /**
         * @brief Serialize NRCstate Packet
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override;

        uint16_t state;
        int32_t value;

        static constexpr size_t size(){
            return getSerializer().member_size();
        }

};

