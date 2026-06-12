#include <Arduino.h>
#include "../../Pins/pins.h"
#include "switch.h"

static SystemSwitchState switch_state = SystemSwitchState::ON;

void system_switch_init()
{
    pinMode(SWITCH, INPUT_PULLUP);
    switch_state = SystemSwitchState::ON;
}

bool system_switch_pressed()
{
    return digitalRead(SWITCH) == LOW;
}

void system_switch_update(
    bool motor_standing_still,
    bool at_far_end
)
{
    if (system_switch_pressed())
    {
        if (switch_state == SystemSwitchState::ON)
        {
            switch_state = SystemSwitchState::SHUTDOWN_REQUESTED;
        }
    }

    if (switch_state == SystemSwitchState::SHUTDOWN_REQUESTED)
    {
        // Later: command motor to move to far end
        switch_state = SystemSwitchState::MOVING_TO_FAR_END;
    }

    if (switch_state == SystemSwitchState::MOVING_TO_FAR_END)
    {
        if (at_far_end && motor_standing_still)
        {
            switch_state = SystemSwitchState::READY_TO_POWER_OFF;
        }
    }

    if (switch_state == SystemSwitchState::READY_TO_POWER_OFF)
    {
        // Later: cut power / disable motor driver
        switch_state = SystemSwitchState::OFF;
    }
}

SystemSwitchState system_switch_get_state()
{
    return switch_state;
}

bool system_switch_is_on()
{
    return switch_state == SystemSwitchState::ON;
}

bool system_switch_shutdown_requested()
{
    return switch_state == SystemSwitchState::SHUTDOWN_REQUESTED ||
           switch_state == SystemSwitchState::MOVING_TO_FAR_END;
}

bool system_switch_ready_to_power_off()
{
    return switch_state == SystemSwitchState::READY_TO_POWER_OFF ||
           switch_state == SystemSwitchState::OFF;
}