#include "nrccrosshair.h"

#include <libriccore/riccoresystem.h>

#include "Config/pinmap_config.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"

NRCCrosshair::NRCCrosshair(RnpNetworkManager& networkmanager, Types::CrosshairTypes_t::SystemStatus_t& systemstatus, Types::CoreTypes::CommandHandler_t& commandHandler, Types::CrosshairTypes_t::StateMachine_t& statemachine):
        NRCRemoteActuatorBase(networkmanager),
        networkmanager(networkmanager),
        commandHandler(commandHandler),
        systemstatus(systemstatus),
        spiBaro(1),
        baro(spiBaro, systemstatus, PinMap::BARO_CS),
        baroData({ 0 }),
        smoothedBaroAlt(0),
        // logicRail("Logic VRailMonitor", PinMap::BatteryV, GeneralConfig::LOGIC_r1, GeneralConfig::LOGIC_r2),
        qdRail("QD VRailMonitor", PinMap::QDV, GeneralConfig::DEPLOY_r1, GeneralConfig::DEPLOY_r2),
        lowVoltageTriggered(false),
        pyro(PinMap::PyroNuke, PinMap::PyroCont, networkmanager),
        pyroAdapter(static_cast<uint8_t>(Services::ID::Pyro), pyro, [](const std::string& msg){RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(msg);}),
        stateMachine(statemachine),
        vspi(0),
        filestore(vspi, PinMap::SD_CS, SD_SCK_MHZ(20), false, &systemstatus) {}

void NRCCrosshair::setup() {
    vspi.begin(PinMap::SD_SCLK,PinMap::SD_MISO,PinMap::SD_MOSI);
    vspi.setFrequency(SD_SCK_MHZ(20));

    pinMode(PinMap::SD_CS, OUTPUT);
    digitalWrite(PinMap::SD_CS, HIGH);

    spiBaro.begin(PinMap::BARO_SCLK, PinMap::BARO_MISO, PinMap::BARO_MOSI);

    baro.setup();
    baro.calibrateBaro();

    // logicRail.setup(GeneralConfig::LOGIC_MAX_V, GeneralConfig::LOGIC_LOW_V, GeneralConfig::LOGIC_MIN_V);

    qdRail.setup(GeneralConfig::DEPLOY_MAX_V, GeneralConfig::DEPLOY_LOW_V, GeneralConfig::DEPLOY_MIN_V);

    // Initialise state machine
    stateMachine.initalize(std::make_unique<Idle>(systemstatus, commandHandler));

    try {
        logFilePath = filestore.generateUniquePath("/logs", GeneralConfig::LogFilename);
        std::unique_ptr<WrappedFile> logFile = filestore.open(logFilePath);
        logToFile = fileLogger.initialize(std::move(logFile));
    } catch (std::exception& e) {
        Serial.println((" ----- SD CARD NOT AVAILABLE ----- : " + std::string(e.what())).c_str());
    }

    pyro.setup();
}

void NRCCrosshair::update() {
    // Read QD voltage and test for low voltage
    float voltage;
    if (qdRail.update(voltage)) {
        qdRailVoltage = voltage;
        lowVoltageTriggered = qdRail._lowVoltageTriggered;

        logFrame.qdVoltageMV = static_cast<uint32_t>(qdRailVoltage * 1000);
        logFrame.timestamp = millis();
    }

    // Read in barometer data
    float prevBaroAlt = baroData.alt;
    baro.update(baroData);
    // Simple exponential smoothing
    smoothedBaroAlt = prevBaroAlt * 0.6 + baroData.alt * 0.4;

    // If correct time then log to SD
    static unsigned long lastTimeLogged = 0;
    if (logToFile && (systemstatus.flagSet(SYSTEM_FLAG::STATE_ARMED) || systemstatus.flagSet(SYSTEM_FLAG::STATE_DEPLOY)) && logFrame.timestamp - lastTimeLogged < GeneralConfig::SD_LOG_INTERVAL) {
        lastTimeLogged = millis();

        logFrame.deployed = deployed;
        fileLogger.log(logFrame);
    }
}

void NRCCrosshair::arm_base(int32_t arg) {
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Arming Crosshair");
    baro.calibrateBaro();

    // Call parent arm
    NRCRemoteActuatorBase<NRCCrosshair>::arm_base(arg);

    // Arm the pyro and re-calibrate the barometer
    // Indicates that the module is ready to launch
    pyroAdapter.arm(arg);

    // If the pyro adapter is
    // if (pyroAdapter.getState().flagSet(LIBRRC::COMPONENT_STATUS_FLAGS::NOMINAL)) {
    networkmanager.registerService(static_cast<uint8_t>(Services::ID::Pyro), pyro.getThisNetworkCallback());
    stateMachine.changeState(std::make_unique<Armed>(systemstatus, commandHandler, *this));
    // }
}

void NRCCrosshair::disarm_base() {
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Disarming Crosshair");

    // Disarm base
    NRCRemoteActuatorBase<NRCCrosshair>::disarm_base();

    // Disarm pyro and remove from network manager
    pyroAdapter.disarm();
    networkmanager.unregisterService(static_cast<uint8_t>(Services::ID::Pyro));

    // Go back to idle state
    stateMachine.changeState(std::make_unique<Idle>(systemstatus, commandHandler));
}

void NRCCrosshair::execute_base(int32_t arg) {

    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Executing arg " + std::to_string(arg));

    switch (arg) {
        case 1: // Default
            if (systemstatus.flagSet(SYSTEM_FLAG::STATE_IDLE)) break;
            disarm_base();
            break;

        case 2: // Armed
            if (systemstatus.flagSet(SYSTEM_FLAG::STATE_ARMED)) break;
            arm_base(0);
            break;

        case 3: // Deploy
            if (systemstatus.flagSet(SYSTEM_FLAG::STATE_DEPLOY) || systemstatus.flagSet(SYSTEM_FLAG::STATE_IDLE)) {
                if (systemstatus.flagSet(SYSTEM_FLAG::STATE_DEPLOY)) {
                    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("DEPLOYED ALREADY");
                }
                if (systemstatus.flagSet(SYSTEM_FLAG::STATE_IDLE)) {
                    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("IDLE ALREADY");
                }
                break;
            }
            stateMachine.changeState(std::make_unique<Deploy>(systemstatus, commandHandler, *this));
            break;

        // case 4: // Debug
        //     if (systemstatus.flagSet(SYSTEM_FLAG::STATE_DEBUG)) break;
        //     stateMachine.changeState(std::make_unique<Debug>(m_PyroInitParams, m_networkmanager, *this));
        //     break;

        default:
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Unknown command ID!");
            break;
    }
}
