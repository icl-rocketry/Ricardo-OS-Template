#pragma once
#include "rnp_packet.h"
#include "rnp_networkmanager.h"

class stateMachine; //forward declaration
namespace Commands{
    enum class ID:uint8_t{
        Nocommand = 0,
        Launch = 1, 
        Reset = 2,
        Abort = 3,
        Set_Home = 4,
        Start_Logging = 5,
        Stop_Logging = 6,
        Telemetry = 8,
        Clear_Flash = 10,
        Clear_SD = 11,
        Print_Flash_filesystem = 12,
        Print_Sd_filesystem = 13,
        Play_Song = 14,
        Skip_Song = 15,
        Clear_Song_Queue = 16,
        Reset_Orientation = 50,
        Reset_Localization = 51,
        Set_Beta = 52,
        Calibrate_AccelGyro_Bias = 60,
        Calibrate_Mag_Full = 61,
        Calibrate_Baro = 62,
        Ignition = 69,
        Enter_Debug = 100,
        Enter_Preflight = 101,
        // Enter_Groundstation = 102,
        Enter_Countdown = 103,
        Enter_Flight = 104,
        Enter_Recovery = 105,
        Exit_Debug = 106,
        Exit_to_Debug = 107,
        Engine_Info = 180,
        Set_Throttle  = 181,
        Pyro_info = 200,
        Fire_pyro = 201,
        Free_Ram = 250
    };

    void LaunchCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ResetCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void AbortCommand(stateMachine& sm, const  RnpPacketSerialized& packet);
    void SetHomeCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void StartLoggingCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void StopLoggingCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void TelemetryCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ClearFlashCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ClearSDCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void PlaySongCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void SkipSongCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ClearSongQueueCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ResetOrientationCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ResetLocalizationCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void SetBetaCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void CalibrateAccelGyroBiasCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void CalibrateMagFullCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void CalibrateBaroCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void IgnitionCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void EnterDebugCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void EnterPreflightCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    // void EnterGroundstationCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void EnterCountdownCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void EnterFlightCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void EnterRecoveryCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ExitDebugCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void ExitToDebugCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void EngineInfoCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void SetThrottleCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void PyroInfoCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void FireInfoCommand(stateMachine& sm, const RnpPacketSerialized& packet);
    void FreeRamCommand(stateMachine& sm, const RnpPacketSerialized& packet);

}

// enum class COMMANDS:uint8_t {
//     Nocommand = 0,
//     Launch = 1, 
//     Reset = 2,
//     Abort = 3,
//     Set_Home = 4,
//     Start_Logging = 5,
//     Stop_Logging = 6,
//     Telemetry = 8,
//     Clear_Flash = 10,
//     Clear_SD = 11,
//     Print_Flash_filesystem = 12,
//     Print_Sd_filesystem = 13,
//     Play_Song = 14,
//     Skip_Song = 15,
//     Clear_Song_Queue = 16,
//     Reset_Orientation = 50,
//     Reset_Localization = 51,
//     Set_Beta = 52,
//     Calibrate_AccelGyro_Bias = 60,
//     Calibrate_Mag_Full = 61,
//     Calibrate_Baro = 62,
//     Ignition = 69,
//     Enter_Debug = 100,
//     Enter_Preflight = 101,
//     // Enter_Groundstation = 102,
//     Enter_Countdown = 103,
//     Enter_Flight = 104,
//     Enter_Recovery = 105,
//     Exit_Debug = 106,
//     Exit_to_Debug = 107,
//     Engine_Info = 180,
//     Set_Throttle  = 181,
//     Pyro_info = 200,
//     Fire_pyro = 201,
//     Free_Ram = 250
// };






