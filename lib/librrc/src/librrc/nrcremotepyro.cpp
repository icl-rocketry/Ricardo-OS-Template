#include "nrcremotepyro.h"
#include <esp_task.h>
#include <functional>
#include <Arduino.h>



void NRCRemotePyro::setup()
{
    pinMode(_firePin,OUTPUT);
    digitalWrite(_firePin,LOW);
    pinMode(_contPin,INPUT);
    _state.newFlag(COMPONENT_STATUS_FLAGS::DISARMED);
    updateContinuity();
}


void NRCRemotePyro::arm_impl(packetptr_t packetptr)
{
    updateContinuity();
    NRCRemoteActuatorBase::arm_impl(std::move(packetptr));
}

void NRCRemotePyro::updateContinuity()
{
    if (digitalRead(_contPin))
    {
        if (_state.flagSet(COMPONENT_STATUS_FLAGS::ERROR_CONTINUITY))
        {
            _state.deleteFlag(COMPONENT_STATUS_FLAGS::ERROR_CONTINUITY);
        }

    }
    else
    {
        if (!_state.flagSet(COMPONENT_STATUS_FLAGS::ERROR_CONTINUITY)){
            _state.newFlag(COMPONENT_STATUS_FLAGS::ERROR_CONTINUITY);
        }
    }
}


void NRCRemotePyro::execute_impl(packetptr_t packetptr)
{
    SimpleCommandPacket execute_command(*packetptr);

    digitalWrite(_firePin,HIGH);

    struct TaskData_t{
        uint8_t firePin;
        uint32_t param;
    };

    TaskData_t taskdata{_firePin,execute_command.arg};

    if ((async_off_task_handle != nullptr) && (eTaskGetState(async_off_task_handle) != eTaskState::eDeleted))
    {
        vTaskDelete(async_off_task_handle); // remove previous running task and replace with new task
    }

    // spawn task to switch off pin given timeout param wiht higher prioirty so sleeping starts immediatley and non static task data is copied immediatley
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
                                // create local copy of data as task data is not static
                                TaskData_t taskdata = *reinterpret_cast<TaskData_t *>(pvParameters);

                                TickType_t xLastWakeTime = xTaskGetTickCount();
                                vTaskDelayUntil(&xLastWakeTime, taskdata.param / portTICK_PERIOD_MS); // sleep for required amount
                                digitalWrite(taskdata.firePin,LOW);
                                vTaskDelete(NULL); // delete task 
                            },
                            "nukeofftask",
                            1000,
                            (void *)&taskdata,
                            2,
                            &async_off_task_handle,
                            1);


}

void NRCRemotePyro::getstate_impl(packetptr_t packetptr)
{
    updateContinuity();
    NRCRemoteBase::getstate_impl(std::move(packetptr));
}