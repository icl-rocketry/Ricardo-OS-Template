#include "commands.h"
#include "packets/TelemetryPacket.h"
#include "Sensors/mmc5983ma.h"
#include "Sensors/sensorStructs.h"
#include "Storage/logController.h"

#include "States/debug.h"
#include "States/groundstation.h"
#include "States/launch.h"
#include "States/flight.h"
#include "States/recovery.h"
#include "States/preflight.h"
#include "rnp_packet.h"
#include "rnp_interface.h"
#include "Network/interfaces/radio.h"
#include "packets/magcalcommandpacket.h"

#include "commandHandler.h"

#include "stateMachine.h"


void Commands::LaunchCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	sm.changeState(new Launch(&sm));
}

void Commands::ResetCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{	
	ESP.restart(); 
}

void Commands::AbortCommand(stateMachine& sm,const  RnpPacketSerialized& packet) 
{
	if(sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_LAUNCH)){
		//check if we are in no abort time region
		//close all valves
		sm.changeState(new Preflight(&sm));
	}else if (sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_FLIGHT)){
		//this behaviour needs to be confirmed with recovery 
		//might be worth waiting for acceleration to be 0 after rocket engine cut
		sm.changeState(new Recovery(&sm));
	}
}

void Commands::SetHomeCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.estimator.setHome(sm.sensors.getData());
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete
	
}

void Commands::StartLoggingCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	SimpleCommandPacket commandpacket(packet);
	sm.logcontroller.startLogging((LOG_TYPE)commandpacket.arg);
}

void Commands::StopLoggingCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	SimpleCommandPacket commandpacket(packet);
	sm.logcontroller.stopLogging((LOG_TYPE)commandpacket.arg);
}

void Commands::TelemetryCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	SimpleCommandPacket commandpacket(packet);

	TelemetryPacket telemetry;

	auto raw_sensors = sm.sensors.getData();
	auto estimator_state = sm.estimator.getData();

	telemetry.header.type = static_cast<uint8_t>(CommandHandler::PACKET_TYPES::TELEMETRY_RESPONSE);
	telemetry.header.source = sm.networkmanager.getAddress();
	// this is not great as it assumes a single command handler with the same service ID
	// would be better if we could pass some context through the function paramters so it has an idea who has called it
	// or make it much clearer that only a single command handler should exist in the system
	telemetry.header.source_service = sm.commandhandler.getServieID();
	telemetry.header.destination = commandpacket.header.source;
	telemetry.header.destination_service = commandpacket.header.source_service;
	telemetry.header.uid = commandpacket.header.uid; 
	telemetry.system_time = millis();

	telemetry.pn = estimator_state.position(0);
	telemetry.pe = estimator_state.position(1);
	telemetry.pd = estimator_state.position(2);

	telemetry.vn = estimator_state.velocity(0);
	telemetry.ve = estimator_state.velocity(1);
	telemetry.vd = estimator_state.velocity(2);

	telemetry.an = estimator_state.acceleration(0);
	telemetry.ae = estimator_state.acceleration(1);
	telemetry.ad = estimator_state.acceleration(2);

	telemetry.roll = estimator_state.eulerAngles(0);
	telemetry.pitch = estimator_state.eulerAngles(1);
	telemetry.yaw =estimator_state.eulerAngles(2);

	telemetry.q0 = estimator_state.orientation.w();
	telemetry.q1 = estimator_state.orientation.x();
	telemetry.q2 =estimator_state.orientation.y();
	telemetry.q3 =estimator_state.orientation.z();

	telemetry.lat = raw_sensors.gps.lat;
	telemetry.lng = raw_sensors.gps.lng;
	telemetry.alt = raw_sensors.gps.alt;
	telemetry.sat = raw_sensors.gps.sat;

	telemetry.ax = raw_sensors.accelgyro.ax;
	telemetry.ay = raw_sensors.accelgyro.ay;
	telemetry.az = raw_sensors.accelgyro.az;

	telemetry.h_ax = raw_sensors.accel.ax;
	telemetry.h_ay = raw_sensors.accel.ay;
	telemetry.h_az = raw_sensors.accel.az;

	telemetry.gx = raw_sensors.accelgyro.gx;
	telemetry.gy = raw_sensors.accelgyro.gy;
	telemetry.gz = raw_sensors.accelgyro.gz;

	telemetry.mx = raw_sensors.mag.mx;
	telemetry.my = raw_sensors.mag.my;
	telemetry.mz = raw_sensors.mag.mz;

	telemetry.baro_temp = raw_sensors.baro.temp;
	telemetry.baro_press = raw_sensors.baro.press;
	telemetry.baro_alt = raw_sensors.baro.alt;

	telemetry.batt_voltage = raw_sensors.batt.volt;
	telemetry.batt_percent= raw_sensors.batt.percent;

	telemetry.launch_lat = estimator_state.gps_launch_lat;
	telemetry.launch_lng = estimator_state.gps_launch_long;
	telemetry.launch_alt = estimator_state.gps_launch_alt;

	telemetry.system_status = sm.systemstatus.getStatus();
	

	const RadioInterfaceInfo* radioinfo = static_cast<const RadioInterfaceInfo*>(sm.radio.getInfo());
	telemetry.rssi = radioinfo->rssi;
	telemetry.snr = radioinfo->snr;



	sm.networkmanager.sendPacket(telemetry);

}

void Commands::ClearFlashCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.storagecontroller.erase(STORAGE_DEVICE::FLASH);
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete

}

void Commands::ClearSDCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.storagecontroller.erase(STORAGE_DEVICE::MICROSD);
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete
}

void Commands::PlaySongCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{

	SimpleCommandPacket commandpacket(packet);
	sm.tunezhandler.play_by_idx(commandpacket.arg);
}

void Commands::SkipSongCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT)){
		return;
	}
	sm.tunezhandler.skip();
}

void Commands::ClearSongQueueCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT)){
		return;
	}
	sm.tunezhandler.clear();
}

void Commands::ResetOrientationCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.estimator.resetOrientation();
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete
}

void Commands::ResetLocalizationCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.estimator.setup();
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete
}

void Commands::SetBetaCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
		return;
	}
	SimpleCommandPacket commandpacket(packet);
	float beta = ((float)commandpacket.arg) / 100.0;
	sm.estimator.changeBeta(beta);
}

void Commands::CalibrateAccelGyroBiasCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.sensors.calibrateAccelGyro();
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete
}

void Commands::CalibrateMagFullCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
		return;
	}
	//check packet type received
	if (packet.header.type != static_cast<uint8_t>(CommandHandler::PACKET_TYPES::MAGCAL)){
		//incorrect packet type received do not deserialize
		return;
	}

	MagCalCommandPacket magcalpacket(packet);
	sm.sensors.calibrateMag(MagCalibrationParameters{magcalpacket.fieldMagnitude,
													   magcalpacket.inclination,
													   magcalpacket.declination,
													   magcalpacket.getA(),
													   magcalpacket.getB()});
	sm.tunezhandler.play(MelodyLibrary::confirmation); // play sound when complete
}

void Commands::CalibrateBaroCommand(stateMachine& sm, const RnpPacketSerialized& packet)
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.sensors.calibrateBaro();
	sm.tunezhandler.play(MelodyLibrary::confirmation); //play sound when complete
}

void Commands::IgnitionCommand(stateMachine& sm, const RnpPacketSerialized& packet)
{
	// if(sm.systemstatus.flagSet(SYSTEM_FLAG::STATE_LAUNCH) && !sm.systemstatus.flagSet(SYSTEM_FLAG::ERROR_FLIGHTCHECK))
	if(sm.systemstatus.flagSet(SYSTEM_FLAG::STATE_LAUNCH))
	{
		uint32_t currentTime = millis();
		sm.estimator.setIgnitionTime(currentTime); // set igintion time
		
	}
}

void Commands::EnterDebugCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT)){
		return;
	}
	sm.changeState(new Debug(&sm));
	sm.systemstatus.newFlag(SYSTEM_FLAG::DEBUG);
}

void Commands::EnterPreflightCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.changeState(new Preflight(&sm));
}

// void Commands::EnterGroundstationCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
// {
// 	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_PREFLIGHT,SYSTEM_FLAG::DEBUG)){
// 		return;
// 	}
// 	sm.changeState(new Groundstation(&sm));
// }

void Commands::EnterCountdownCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.changeState(new Launch(&sm));
}

void Commands::EnterFlightCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.changeState(new Flight(&sm));
}

void Commands::EnterRecoveryCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.changeState(new Recovery(&sm));
}

void Commands::ExitDebugCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.changeState(new Debug(&sm));
	sm.systemstatus.deleteFlag(SYSTEM_FLAG::DEBUG);
	sm.changeState(new Preflight(&sm));
}

void Commands::ExitToDebugCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::DEBUG)){
		return;
	}
	sm.changeState(new Debug(&sm));
}

void Commands::EngineInfoCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	
}

void Commands::SetThrottleCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_DEBUG)){
		return;
	}
}

void Commands::PyroInfoCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	
}

void Commands::FireInfoCommand(stateMachine& sm, const RnpPacketSerialized& packet) 
{
	if(!sm.systemstatus.flagSetOr(SYSTEM_FLAG::STATE_DEBUG)){
		return;
	}
};

void Commands::FreeRamCommand(stateMachine& sm, const RnpPacketSerialized& packet)
{	
	//avliable in all states
	//returning as simple string packet for ease
	//currently only returning free ram
	MessagePacket_Base<0,static_cast<uint8_t>(CommandHandler::PACKET_TYPES::MESSAGE_RESPONSE)> message("FreeRam: " + std::to_string(esp_get_free_heap_size()));
	// this is not great as it assumes a single command handler with the same service ID
	// would be better if we could pass some context through the function paramters so it has an idea who has called it
	// or make it much clearer that only a single command handler should exist in the system
	message.header.source_service = sm.commandhandler.getServieID(); 
	
	
	message.header.destination_service = packet.header.source_service;
	message.header.source = packet.header.destination;
	message.header.destination = packet.header.source;
	message.header.uid = packet.header.uid;
	sm.networkmanager.sendPacket(message);
	
}
