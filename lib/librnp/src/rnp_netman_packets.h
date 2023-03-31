#pragma once 

#include "rnp_packet.h"
#include "rnp_header.h"
#include "rnp_serializer.h"
#include "rnp_routingtable.h"
#include "rnp_networkmanager.h"

#include <array>


enum class NETMAN_TYPES:uint8_t{
    PING_REQ = 1,
    PING_RES = 2,
    SET_ADDRESS = 3,
    SET_ROUTE = 4,
    SET_TYPE = 5,
    SET_NOROUTEACTION = 6,
    SET_ROUTEGEN = 7,
    SAVE_CONF = 8, // this may benefit from a securtiy key
    RESET_NETMAN = 9 // ^^

};


template<uint8_t TYPE>
using GenericRnpPacket_Base = BasicDataPacket<uint32_t,TYPE,static_cast<uint8_t>(DEFAULT_SERVICES::NETMAN)>;

using GenericRnpPacket = GenericRnpPacket_Base<0>; //when we are reading data we dont care about the type 

using PingPacket = GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::PING_REQ)>;
using SetAddressPacket = GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_ADDRESS)>;
using SetTypePacket = GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_TYPE)>;
using SetNoRouteActionPacket = GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_NOROUTEACTION)>;
using SetRouteGenPacket = GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_ROUTEGEN)>;


class SetRoutePacket: public RnpPacket{
    enum class ADDRESS_TYPE{
        NOTYPE = 0,
        STRING = 1
    };
    private:
        static constexpr auto getSerializer()
        {
            auto ret = RnpSerializer(
                &SetRoutePacket::destination,
                &SetRoutePacket::iface,
                &SetRoutePacket::metric,
                &SetRoutePacket::address_type,
                &SetRoutePacket::address_len
            );
            return ret;
        }
    public:
        ~SetRoutePacket();

        /**
         * @brief Construct a new Ping Packet object
         * 
         * @param sys_time 
         */
        SetRoutePacket(uint8_t destination,Route& route);

        /**
         * @brief Deserialize Ping Packet
         * 
         * @param packet 
         */
        SetRoutePacket(RnpPacketSerialized& packet);

        /**
         * @brief Serialize into buf
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override;

        /**
         * @brief Returns the data in a route object for easier insertion into routing table
         * 
         * @return Route 
         */
        Route getRoute();

        //data members
        uint8_t destination;

        uint8_t iface;
        uint8_t metric;

        uint8_t address_type;
        uint8_t address_len;
        
        std::array<uint8_t,32> address_data; //address payload

    
        
        static constexpr size_t size(){
            return getSerializer().member_size() + sizeof(address_data);
        };
};

