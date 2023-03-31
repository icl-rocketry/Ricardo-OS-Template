#include "commandHandler.h"

#include <vector>
#include <functional>
#include <memory>

#include "stateMachine.h"

#include "commands.h"

#include "flags.h"

CommandHandler::CommandHandler(stateMachine *sm) : RnpNetworkService(serviceID),
													_sm(sm),
												   _commandMap{
													   {Commands::ID::Launch, Commands::LaunchCommand},
													   {Commands::ID::Reset, Commands::ResetCommand},
													   {Commands::ID::Abort, Commands::AbortCommand},
													   {Commands::ID::Set_Home, Commands::SetHomeCommand},
													   {Commands::ID::Start_Logging, Commands::StartLoggingCommand},
													   {Commands::ID::Stop_Logging, Commands::StopLoggingCommand},
													   {Commands::ID::Telemetry, Commands::TelemetryCommand},
													   {Commands::ID::Clear_Flash, Commands::ClearFlashCommand},
													   {Commands::ID::Clear_SD, Commands::ClearSDCommand},
													   {Commands::ID::Play_Song, Commands::PlaySongCommand},
													   {Commands::ID::Skip_Song, Commands::SkipSongCommand},
													   {Commands::ID::Clear_Song_Queue, Commands::ClearSongQueueCommand},
													   {Commands::ID::Calibrate_AccelGyro_Bias, Commands::CalibrateAccelGyroBiasCommand},
													   {Commands::ID::Calibrate_Mag_Full, Commands::CalibrateMagFullCommand},
													   {Commands::ID::Calibrate_Baro, Commands::CalibrateBaroCommand},
													   {Commands::ID::Ignition, Commands::IgnitionCommand},
													   {Commands::ID::Set_Beta, Commands::SetBetaCommand},
													   {Commands::ID::Reset_Orientation, Commands::ResetOrientationCommand},
													   {Commands::ID::Reset_Localization, Commands::ResetLocalizationCommand},
													   {Commands::ID::Enter_Debug, Commands::EnterDebugCommand},
													   {Commands::ID::Enter_Preflight, Commands::EnterPreflightCommand},
													   {Commands::ID::Enter_Countdown, Commands::EnterCountdownCommand},
													   {Commands::ID::Enter_Flight, Commands::EnterFlightCommand},
													   {Commands::ID::Enter_Recovery, Commands::EnterRecoveryCommand},
													   {Commands::ID::Exit_Debug, Commands::ExitDebugCommand},
													   {Commands::ID::Exit_to_Debug, Commands::ExitDebugCommand},
													   {Commands::ID::Set_Throttle, Commands::SetThrottleCommand},
													   {Commands::ID::Engine_Info, Commands::EngineInfoCommand},
													   {Commands::ID::Pyro_info, Commands::PyroInfoCommand},
													   {Commands::ID::Fire_pyro, Commands::FireInfoCommand},
													   {Commands::ID::Free_Ram, Commands::FreeRamCommand}},
													   _enabledCommands(_alwaysEnabledCommands) {};

void CommandHandler::handleCommand(std::unique_ptr<RnpPacketSerialized> packetptr)
{
	
	command_t cmd = CommandPacket::getCommand(*packetptr);
	_commandMap.at(static_cast<Commands::ID>(cmd))(*_sm,*packetptr);
	// if (_enabledCommands.test(cmd))
	// {
		
	// }
}

