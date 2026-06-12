#pragma once

#pragma once

struct ControlPanelState
{
    bool slow_pressed = false;
    bool fast_pressed = false;
    bool emergency_pressed = false;
    bool master_switch_on = false;

    int slider_a_raw = 0;
    int slider_b_raw = 0;

    float slider_a_percent = 0.0f;
    float slider_b_percent = 0.0f;

    bool system_switch_pressed = false;
    bool shutdown_requested = false;
    bool ready_to_power_off = false;
};

void control_panel_init();

ControlPanelState read_control_panel();

void update_control_panel_leds(
    bool stop_active,
    bool slow_active,
    bool fast_active
);

void control_panel_apply_ui_fast();
void control_panel_apply_ui_slow();
void control_panel_apply_ui_emergency(bool active);
void control_panel_apply_ui_switch(bool enabled);
void control_panel_apply_ui_position_percent(float percent);
void control_panel_clear_ui_position();

void control_panel_init();

ControlPanelState read_control_panel();

void update_control_panel_leds(
    bool stop_active,
    bool slow_active,
    bool fast_active
);

void control_panel_apply_ui_fast();
void control_panel_apply_ui_slow();
void control_panel_apply_ui_emergency(bool active);
void control_panel_apply_ui_switch(bool enabled);
void control_panel_apply_ui_position_percent(float percent);
void control_panel_clear_ui_position();