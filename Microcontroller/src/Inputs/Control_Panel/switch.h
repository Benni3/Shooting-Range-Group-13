#pragma once

enum class SystemSwitchState
{
    ON,
    SHUTDOWN_REQUESTED,
    MOVING_TO_FAR_END,
    READY_TO_POWER_OFF,
    OFF
};

void system_switch_init();

bool system_switch_pressed();

void system_switch_update(
    bool motor_standing_still,
    bool at_far_end
);

SystemSwitchState system_switch_get_state();

bool system_switch_is_on();
bool system_switch_shutdown_requested();
bool system_switch_ready_to_power_off();