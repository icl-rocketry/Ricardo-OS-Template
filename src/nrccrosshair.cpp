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
        filestore(vspi, PinMap::SD_CS, SD_SCK_MHZ(20), false, &systemstatus),
        i2c(0) {}

void NRCCrosshair::setup() {
    vspi.begin(PinMap::SD_SCLK,PinMap::SD_MISO,PinMap::SD_MOSI);
    vspi.setFrequency(SD_SCK_MHZ(20));

    pinMode(PinMap::SD_CS, OUTPUT);
    digitalWrite(PinMap::SD_CS, HIGH);

    filestore.setup();

    spiBaro.begin(PinMap::BARO_SCLK, PinMap::BARO_MISO, PinMap::BARO_MOSI);

    baro.setup();
    baro.calibrateBaro();

    // logicRail.setup(GeneralConfig::LOGIC_MAX_V, GeneralConfig::LOGIC_LOW_V, GeneralConfig::LOGIC_MIN_V);

    qdRail.setup(GeneralConfig::DEPLOY_MAX_V, GeneralConfig::DEPLOY_LOW_V, GeneralConfig::DEPLOY_MIN_V);

    // Initialise state machine
    stateMachine.initalize(std::make_unique<Idle>(systemstatus, commandHandler));

    try {
        filestore.mkdir("/logs");
        logFilePath = filestore.generateUniquePath("/logs", GeneralConfig::LogFilename);

        std::unique_ptr<WrappedFile> logFile = filestore.open(logFilePath + ".txt", static_cast<FILE_MODE>(O_WRITE | O_CREAT | O_AT_END));
        logToFile = fileLogger.initialize(std::move(logFile));

        if (!logToFile) {
            Serial.println("SD CARD STILL BAD");
        }
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
    }

    // Read in barometer data
    float prevBaroAlt = baroData.alt;
    baro.update(baroData);
    // Simple exponential smoothing
    smoothedBaroAlt = prevBaroAlt * 0.6 + baroData.alt * 0.4;

    // If correct time then log to SD
    static unsigned long lastTimeLogged = 0;
    if (logToFile && millis() - lastTimeLogged > GeneralConfig::SD_LOG_INTERVAL) {
        lastTimeLogged = millis();
        logFrame.qdVoltageMV = static_cast<uint32_t>(qdRailVoltage * 1000);
        logFrame.timestamp = millis();
        logFrame.deployed = deployed;
        logFrame.baroAlt = smoothedBaroAlt;
        fileLogger.log(logFrame);
    }

    // static unsigned long lastI2C = 0;
    // if (systemstatus.flagSet(SYSTEM_FLAG::STATE_ARMED) && millis() - lastI2C > 5000) {
    //     lastI2C = millis();

    //     byte error, address;
    //     int nDevices;
    //     Serial.println("Scanning...");
    //     nDevices = 0;
    //     for(address = 1; address < 127; address++ ) {
    //         i2c.beginTransmission(address);
    //         error = i2c.endTransmission();
    //         if (error == 0) {
    //         Serial.print("I2C device found at address 0x");
    //         if (address<16) {
    //             Serial.print("0");
    //         }
    //         Serial.println(address,HEX);
    //         nDevices++;
    //         }
    //         else if (error==4) {
    //         Serial.print("Unknow error at address 0x");
    //         if (address<16) {
    //             Serial.print("0");
    //         }
    //         Serial.println(address,HEX);
    //         }
    //     }
    //     if (nDevices == 0) {
    //         Serial.println("No I2C devices found\n");
    //     }
    //     else {
    //         Serial.println("done\n");
    //     }
    // }
}

void NRCCrosshair::arm_base(int32_t arg) {
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Arming Crosshair");
    baro.calibrateBaro();

    // Write 1 to Cell Switch
    digitalWrite(PinMap::CELL_SWITCH, HIGH);

    delay(50);

    if (!i2c.begin(PinMap::I2C_SDA, PinMap::I2C_SCL, 100'000)) {
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Couldnt start I2C on Crosshair");
        return;
    }

    delay(500);

    uint8_t regVal = 0x0 | (1 << 7) | (1 << 5);

    // 0: success.
    // 1: data too long to fit in transmit buffer.
    // 2: received NACK on transmit of address.
    // 3: received NACK on transmit of data.
    // 4: other error.
    // 5: timeout

    std::function<std::string(uint8_t)> errFunc = [this](uint8_t code){
        switch (code)
        {
        case 0:
            return "success.";
        case 1:
            return "data too long to fit in transmit buffer.";
        case 2:
            return "received NACK on transmit of address.";
        case 3:
            return "received NACK on transmit of data.";
        case 4:
            return "other error.";
        case 5:
            return "timeout";
        default:
            return "unrecognised err code";
        }
    };

    // // Write low byte (0x6A) to 0x00
    // i2c.beginTransmission(GeneralConfig::BUCK_BOOST_I2C_ADDR);
    // i2c.write(0x00);
    // i2c.write(0xff);
    // uint8_t err = 0;
    // if ((err = i2c.endTransmission()) != 0) {
    //     RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to send I2C 1! err : " + std::to_string(err) + " -- " + errFunc(err));
    //     // return;
    // }
    // delay(500);


    // // Write high byte (0x01) to 0x01 — loads both into DAC
    // i2c.beginTransmission(GeneralConfig::BUCK_BOOST_I2C_ADDR);
    // i2c.write(0x01);
    // i2c.write(0b11);
    // if ((err = i2c.endTransmission()) != 0) {
    //     RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to send I2C 2! err : " + std::to_string(err) + " -- " + errFunc(err));
    //     // return;
    // }
    // delay(500);


    i2c.beginTransmission(GeneralConfig::BUCK_BOOST_I2C_ADDR);
    i2c.write(0x6);
    i2c.write(regVal);
    uint8_t err = 0;
    if ((err = i2c.endTransmission()) != 0) {
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Crosshair Failed to send I2C to BuckBoost! err : " + std::to_string(err) + " -- " + errFunc(err));
        return;
    }

    // Arm the pyro and re-calibrate the barometer
    // Indicates that the module is ready to launch
    pyroAdapter.arm(1);

    // If the pyro adapter is
    if (!pyroAdapter.getState().flagSet(LIBRRC::COMPONENT_STATUS_FLAGS::NOMINAL)) {
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Crosshair Pyro adapter state not nominal");
        return;
    }

    networkmanager.registerService(static_cast<uint8_t>(Services::ID::Pyro), pyro.getThisNetworkCallback());
    stateMachine.changeState(std::make_unique<Armed>(systemstatus, commandHandler, *this));

    // Call parent arm
    NRCRemoteActuatorBase<NRCCrosshair>::arm_base(arg);
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

    // Disable Cell Switch
    digitalWrite(PinMap::CELL_SWITCH, LOW);
}

void NRCCrosshair::execute_base(int32_t arg) {

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
                    // RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("DEPLOYED ALREADY");
                }
                if (systemstatus.flagSet(SYSTEM_FLAG::STATE_IDLE)) {
                    // RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("IDLE ALREADY");
                }
                break;
            }
            stateMachine.changeState(std::make_unique<Deploy>(systemstatus, commandHandler, *this));
            break;

        case 4: // Manually turn off cell switch
            digitalWrite(PinMap::CELL_SWITCH, LOW);
            break;

        // case 5: // Debug
        //     if (systemstatus.flagSet(SYSTEM_FLAG::STATE_DEBUG)) break;
        //     stateMachine.changeState(std::make_unique<Debug>(m_PyroInitParams, m_networkmanager, *this));
        //     break;

        default:
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Unknown command ID!");
            break;
    }
}
