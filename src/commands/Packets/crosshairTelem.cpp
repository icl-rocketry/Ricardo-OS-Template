#include "crosshairTelem.h"

#include <librnp/rnp_packet.h>
#include <librnp/rnp_serializer.h>

#include <vector>

CrosshairTelemPacket::~CrosshairTelemPacket()
{};

CrosshairTelemPacket::CrosshairTelemPacket():
RnpPacket(0,
          10,
          size())
{};

CrosshairTelemPacket::CrosshairTelemPacket(const RnpPacketSerialized& packet):
RnpPacket(packet,size())
{
    getSerializer().deserialize(*this,packet.getBody());
};

void CrosshairTelemPacket::serialize(std::vector<uint8_t>& buf){
    RnpPacket::serialize(buf);
	size_t bufsize = buf.size();
	buf.resize(bufsize + size());
	std::memcpy(buf.data() + bufsize,getSerializer().serialize(*this).data(),size());
};