#include "control_panel.h"
#include "buttons.h"
#include "slider.h"
#include "LED.h"

void control_panel_init()
{
    led_show_stop();
}

ControlPanelState read_control_panel()
{
    ControlPanelState state;

    state.slow_pressed = slow_button_pressed();
    state.fast_pressed = fast_button_pressed();
    state.emergency_pressed = emergency_button_pressed();
    state.switch_enabled = switch_enabled();

    state.slider_a_raw = read_slider_a_raw();
    state.slider_b_raw = read_slider_b_raw();

    state.slider_a_percent = read_slider_a_percent();
    state.slider_b_percent = read_slider_b_percent();

    return state;
}

void update_control_panel_leds(
    bool stop_active,
    bool slow_active,
    bool fast_active
)
{
    if (fast_active)
    {
        led_show_fast();
    }
    else if (slow_active)
    {
        led_show_slow();
    }
    else if (stop_active)
    {
        led_show_stop();
    }
    else
    {
        led_all_off();
    }
}

void control_panel_apply_ui_fast()
{
    set_ui_fast_button(true);
    set_ui_slow_button(false);
}

void control_panel_apply_ui_slow()
{
    set_ui_slow_button(true);
    set_ui_fast_button(false);
}

void control_panel_apply_ui_emergency(bool active)
{
    set_ui_emergency_button(active);
}

void control_panel_apply_ui_switch(bool enabled)
{
    set_ui_switch(enabled);
}

void control_panel_apply_ui_position_percent(float percent)
{
    set_ui_slider_a_percent(percent);
}

void control_panel_clear_ui_position()
{
    clear_ui_slider_a();
}