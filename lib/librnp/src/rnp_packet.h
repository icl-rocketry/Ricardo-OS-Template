#pragma once 

#include "rnp_header.h"
#include <vector>

class RnpPacketSerialized; // forward declaration

class RnpPacket {
    public:
        virtual ~RnpPacket();

        /**
         * @brief Construct a new Rnp Packet object
         * 
         * @param packetService 
         * @param packetType 
         * @param packetSize 
         */
        RnpPacket(uint8_t packetService, uint8_t packetType, uint16_t packetSize);

        /**
         * @brief Construct a new Rnp Packet object from a header
         * 
         * @param header 
         */
        RnpPacket(RnpHeader header);

        /**
         * @brief Deserialization Constructor with size checking. Throws std::runtime_error if the expected size
         * Doesnt match the size decoded in the header or in the provided buffer.
         * 
         * @param serializedPacket - const reference to RnpPacketSerialzed object containing raw packet data
         * @param size expected size of packet body
         */
        RnpPacket(const RnpPacketSerialized& serializedPacket,size_t size);

        //Serialization
        /**
         * @brief Serialize packet to provided buffer.
         * If this is called on a non-casted instance, it will only return the serialized header...
         * 
         * @param buf 
         */
        virtual void serialize(std::vector<uint8_t>& buf);

        RnpHeader header; 
};

class RnpPacketSerialized: public RnpPacket {
    public:
        ~RnpPacketSerialized();
        //Deserialization 
        /**
         * @brief Extract header from serialized byte stream
         * 
         * @param bytes byte stream of serialized packet
         */
        RnpPacketSerialized(const std::vector<uint8_t> &bytes);

        //Serialization
        /**
         * @brief Re-Serialize header back into packet. Make sure to do this if the header is modified. 
         * 
         * 
         */
        void reserializeHeader();
        
        /**
         * @brief Copies the internally stored packet to buf. Does automatically re-serialize header into buf
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override;
       
        /**
         * @brief Extract packet body from packet
         * 
         * @return std::vector<uint8_t> 
         */
        std::vector<uint8_t> getBody() const;

        /**
         * @brief Get the size of the packet Body
         * 
         * @return size_t 
         */
        size_t getBodySize() const; 

        std::vector<uint8_t> packet; // contains serialized packet and header

};

/**
 * @brief A templated data packet for basic types
 * 
 * @tparam T 
 */
template<class T,uint8_t SERVICE,uint8_t TYPE>
class BasicDataPacket : public RnpPacket{
    public:
        ~BasicDataPacket(){};

        /**
         * @brief Construct a new Generic Data Packet
         * 
         * @param sys_time 
         */
        BasicDataPacket(T _data):
            RnpPacket(SERVICE,
            TYPE,
            size()),
            data(_data)
        {};

        /**
         * @brief Deserialize Generic Data Packet
         * 
         * @param packet 
         */
        BasicDataPacket(RnpPacketSerialized& packet):
        RnpPacket(packet,size())
        {
            std::memcpy(&data,packet.getBody().data(),size());
        };

        /**
         * @brief Serialize into buf
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override{
            RnpPacket::serialize(buf);
            size_t bufsize = buf.size();
            buf.resize(bufsize + size());
            std::memcpy(buf.data() + bufsize,&data,size());      
        };


        //data members
        T data;

        static constexpr size_t size(){return sizeof(T);};
};

template<uint8_t SERVICE,uint8_t TYPE>
class MessagePacket_Base : public RnpPacket{
    public:
        ~MessagePacket_Base(){};

        /**
         * @brief Construct a new Generic Data Packet
         * 
         * @param sys_time 
         */
        MessagePacket_Base(std::string msg):
            RnpPacket(SERVICE,
            TYPE,
            msg.size()),
            _msg(msg)
        {};

        /**
         * @brief Deserialize Generic Data Packet
         * 
         * @param packet 
         */
        MessagePacket_Base(RnpPacketSerialized& packetData):
        RnpPacket(packetData.header) //message packet has a dynamic size so cant check size of body while deserializing
        {
            _msg.assign(packetData.packet.begin() + header.size(),packetData.packet.end());
        };

        /**
         * @brief Serialize into buf
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override{
            RnpPacket::serialize(buf);
            size_t bufsize = buf.size();
            buf.resize(bufsize + size());
            std::memcpy(buf.data() + bufsize,_msg.data(),size());      
        };


        //data members
        std::string _msg;

        size_t size(){return _msg.size();};
};

using MessagePacket_Default = MessagePacket_Base<0,0>;