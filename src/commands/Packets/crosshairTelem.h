#pragma once

#include <librnp/rnp_packet.h>
#include <librnp/rnp_serializer.h>

#include <vector>

//shamelessly copied from stark's telemetry packet

class CrosshairTelemPacket : public RnpPacket{
    private:
    //serializer framework
        static constexpr auto getSerializer()
        {
            auto ret = RnpSerializer(
                &CrosshairTelemPacket::qdVoltageMV,
                &CrosshairTelemPacket::deployed,
                &CrosshairTelemPacket::system_status,
                &CrosshairTelemPacket::system_time,
                &CrosshairTelemPacket::baro_alt
            );

            return ret;
        }

    public:
        ~CrosshairTelemPacket();

        CrosshairTelemPacket();
        /**
         * @brief Deserialize Telemetry Packet
         *
         * @param data
         */
        CrosshairTelemPacket(const RnpPacketSerialized& packet);

        /**
         * @brief Serialize Telemetry Packet
         *
         * @param buf
         */
        void serialize(std::vector<uint8_t>& buf) override;

        uint32_t qdVoltageMV;
        bool deployed;
        uint32_t system_status;
        uint64_t system_time;
        float baro_alt;

        static constexpr size_t size(){
            return getSerializer().member_size();
        }

};