#include "simplecommandpacket.h"

#include "rnp_packet.h"
#include "rnp_serializer.h"


SimpleCommandPacket::~SimpleCommandPacket()
{};

SimpleCommandPacket::SimpleCommandPacket(uint8_t _command, uint32_t _arg):
RnpPacket(0,
          0,
          size()),
command(_command),
arg(_arg)
{};

SimpleCommandPacket::SimpleCommandPacket(const RnpPacketSerialized& packet):
RnpPacket(packet,size())
{
    getSerializer().deserialize(*this,packet.getBody());
};

void SimpleCommandPacket::serialize(std::vector<uint8_t>& buf){
    RnpPacket::serialize(buf);
	size_t bufsize = buf.size();
	buf.resize(bufsize + size());
	std::memcpy(buf.data() + bufsize,getSerializer().serialize(*this).data(),size());
};


command_t CommandPacket::getCommand(const RnpPacketSerialized& packet) 
{
    const size_t headerSize = packet.header.size();
    command_t commandID;
    std::memcpy(&commandID,packet.packet.data() + headerSize,sizeof(command_t)); // extract command id from any command packet
    return commandID;
}
