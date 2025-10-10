/**
 * @file Armed.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Example implementation of a state. Note the initialize and exit methods do not need to be reimplemented if not needed. However enusre the parent functions are called if they are!
 * @version 0.1
 * @date 2023-06-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <memory>

#include "Config/forward_decl.h"

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>

#include "Config/systemflags_config.h"
#include "Config/general_config.h"
#include "Config/types.h"
#include "Sensors/sensorStructs.h"

#include "nrccrosshair.h"

class Armed : public Types::CoreTypes::State_t {
public:
    /**
     * @brief Armed state constructor. All states require the systemstatus object to be passed in, as well as any other system level objects required. For example, if
     * we want to control the available commands, we need to pass in the command handler from the riccoresystem.
     *
     */
    Armed(Types::CoreTypes::SystemStatus_t& systemtatus, Types::CoreTypes::CommandHandler_t& commandhandler, NRCCrosshair& crosshair);

    /**
     * @brief Perform any initialization required for the state
     *
     */
    void initialize() override;

    /**
     * @brief Function called every update cycle, use to implement periodic actions such as checking sensors. If nullptr is returned, the statemachine will loop the state,
     * otherwise pass a new state ptr to transition to a new state.
     *
     * @return std::unique_ptr<State>
     */
    Types::CoreTypes::State_ptr_t update() override;

    /**
     * @brief Exit state actions, cleanup any files opened, save data that kinda thing.
     *
     */
    void exit() override;

private:
    Types::CrosshairTypes_t::SystemStatus_t& systemstatus;
    Types::CoreTypes::CommandHandler_t& commandhandler;
    uint32_t prevLogMessageTime;

    unsigned long armStartTime;
    unsigned long armIdleTime;

public:
    NRCCrosshair& crosshair;

private:
    bool hasGoneOverDeploymentAlt = false;
    const float deploymentAlt = GeneralConfig::DEPLOYMENT_ALTITUDE; // Altitude to trigger pyro channel at.
    const float dAlt = GeneralConfig::DEPLOYMENT_DALT; // Altitude debounce delta. Positive will only trigger at deployment alt + dAlt, and lower will only trigger at -dAtl.
};