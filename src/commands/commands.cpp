/**
 * @file commands.cpp
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Implementation of commands for system
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "commands.h"

#include <librnp/rnp_packet.h>
#include <libriccore/commands/commandhandler.h>

#include "Commands/Packets/crosshairTelem.h"

#include "system.h"

#include "esp_log.h"

void Commands::FreeRamCommand(System& sm, const RnpPacketSerialized& packet)
{

	// ESP_LOGI("ch", "%s", "deserialize");

	SimpleCommandPacket commandpacket(packet);

	uint32_t freeram = esp_get_free_heap_size();
	//avliable in all states
	//returning as simple string packet for ease
	//currently only returning free ram
	if (commandpacket.arg == 0){
	MessagePacket_Base<0,static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("FreeRam: " + std::to_string(esp_get_free_heap_size()));
	// this is not great as it assumes a single command handler with the same service ID
	// would be better if we could pass some context through the function paramters so it has an idea who has called it
	// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = sm.commandhandler.getServiceID();
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
	else if (commandpacket.arg == 1)
	{
		BasicDataPacket<uint32_t,0,105> responsePacket(freeram);
		responsePacket.header.source_service = sm.commandhandler.getServiceID();
		responsePacket.header.destination_service = packet.header.source_service;
		responsePacket.header.source = packet.header.destination;
		responsePacket.header.destination = packet.header.source;
		responsePacket.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(responsePacket);
	}

}


void Commands::CrosshairTelem(System& sm, const RnpPacketSerialized& packet)
{
	SimpleCommandPacket commandpacket(packet);

	CrosshairTelemPacket crosshairTelem;

	crosshairTelem.header.type = static_cast<uint8_t>(10);
	crosshairTelem.header.source = sm.networkmanager.getAddress();
	crosshairTelem.header.source_service = sm.commandhandler.getServiceID();
	crosshairTelem.header.destination = commandpacket.header.source;
	crosshairTelem.header.destination_service = commandpacket.header.source_service;
	crosshairTelem.header.uid = commandpacket.header.uid;

	crosshairTelem.system_status = sm.crosshair.systemstatus.getStatus();
	crosshairTelem.qdVoltageMV = static_cast<uint32_t>(sm.crosshair.qdRailVoltage * 1000);
	crosshairTelem.deployed = sm.crosshair.deployed;
	crosshairTelem.baro_alt = sm.crosshair.smoothedBaroAlt;
	crosshairTelem.system_time = millis();

	sm.networkmanager.sendPacket(crosshairTelem);

}