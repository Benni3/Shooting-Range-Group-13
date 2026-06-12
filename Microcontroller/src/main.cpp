#include <Arduino.h>

#include "handshake.h"
#include "Pins/pin.h"
#include "Inputs/Control_Panel/control_panel.h"
#include "Stream/downstream.h"
#include "Stream/upstream.h"

ControlPanelState last_panel;
bool has_last_panel = false;

bool target_controlled_by_ui = false;
float slider_value_when_ui_took_control = 0.0f;

const float SLIDER_NOISE_TOLERANCE = 0.05f;
const float PHYSICAL_SLIDER_TAKEOVER_DELTA = 2.0f;

bool approximately_changed(float a, float b, float tolerance = SLIDER_NOISE_TOLERANCE)
{
    return abs(a - b) > tolerance;
}

void handle_downstream_command(const DownstreamCommand& command)
{
    if (downstream_is_none(command)) return;

    if (downstream_is_handshake(command))
    {
        Serial.println(get_project_id());
    }
    else if (downstream_is_ping(command))
    {
        Serial.println("PONG");
    }
    else if (downstream_is_fast(command))
    {
        control_panel_apply_ui_fast();
        upstream_state.fast_active = true;
        upstream_state.slow_active = false;
        upstream_state.stop_active = false;
        upstream_state.mode = "FAST";
    }
    else if (downstream_is_slow(command))
    {
        control_panel_apply_ui_slow();
        upstream_state.fast_active = false;
        upstream_state.slow_active = true;
        upstream_state.stop_active = false;
        upstream_state.mode = "SLOW";
    }
    else if (downstream_is_emergency_stop(command))
    {
        control_panel_apply_ui_emergency(true);
        upstream_state.emergency_active = true;
        upstream_state.fast_active = false;
        upstream_state.slow_active = false;
        upstream_state.stop_active = true;
        upstream_state.pwm = 0;
        upstream_state.direction = "STOP";
        upstream_state.mode = "EMERGENCY_STOP";
    }
    else if (downstream_is_clear_emergency_stop(command))
    {
        control_panel_apply_ui_emergency(false);
        upstream_state.emergency_active = false;
        upstream_state.stop_active = true;
        upstream_state.mode = "IDLE";
    }
    else if (downstream_is_position(command))
    {
        upstream_state.target_position = constrain(command.value, 0.0f, 100.0f);

        ControlPanelState panel = read_control_panel();
        slider_value_when_ui_took_control = panel.slider_a_percent;
        target_controlled_by_ui = true;
    }
    else if (downstream_is_manual_pwm(command))
    {
        upstream_state.pwm = constrain((int)command.value, 0, 255);
    }
    else if (downstream_is_manual_forward(command))
    {
        upstream_state.direction = "FORWARD";
        upstream_state.forward_active = true;
        upstream_state.back_active = false;
    }
    else if (downstream_is_manual_backward(command))
    {
        upstream_state.direction = "BACK";
        upstream_state.forward_active = false;
        upstream_state.back_active = true;
    }
    else if (downstream_is_manual_stop(command))
    {
        upstream_state.direction = "STOP";
        upstream_state.forward_active = false;
        upstream_state.back_active = false;
        upstream_state.pwm = 0;
    }
    else if (downstream_is_recalibrate_zero(command))
    {
        upstream_state.actual_position = 0.0f;
        upstream_state.rotational_position = 0.0f;
        upstream_state.horizontal_speed = 0.0f;
        upstream_state.horizontal_acceleration = 0.0f;
        upstream_state.angular_speed = 0.0f;
        upstream_state.angular_acceleration = 0.0f;
        Serial.println("ZERO_OK");
    }
    else if (downstream_is_set_length(command))
    {
        Serial.print("LENGTH_SET:");
        Serial.println(command.value, 2);
    }
    else
    {
        Serial.print("UNKNOWN_COMMAND: ");
        Serial.println(command.raw);
    }
}

void apply_panel_mode(const ControlPanelState& panel)
{
    if (panel.emergency_pressed)
    {
        upstream_state.emergency_active = true;
        upstream_state.fast_active = false;
        upstream_state.slow_active = false;
        upstream_state.stop_active = true;
        upstream_state.mode = "EMERGENCY_STOP";
        upstream_state.direction = "STOP";
        upstream_state.pwm = 0;
        return;
    }

    if (upstream_state.emergency_active) return;

    if (panel.fast_pressed)
    {
        upstream_state.fast_active = true;
        upstream_state.slow_active = false;
        upstream_state.stop_active = false;
        upstream_state.mode = "FAST";
    }
    else if (panel.slow_pressed)
    {
        upstream_state.fast_active = false;
        upstream_state.slow_active = true;
        upstream_state.stop_active = false;
        upstream_state.mode = "SLOW";
    }
    else
    {
        upstream_state.fast_active = false;
        upstream_state.slow_active = false;
        upstream_state.stop_active = true;
        upstream_state.mode = "IDLE";
    }
}

void update_from_control_panel()
{
    ControlPanelState panel = read_control_panel();

    bool first_read = !has_last_panel;

    bool buttons_changed =
        first_read ||
        panel.fast_pressed != last_panel.fast_pressed ||
        panel.slow_pressed != last_panel.slow_pressed ||
        panel.emergency_pressed != last_panel.emergency_pressed ||
        panel.master_switch_on != last_panel.master_switch_on;

    bool slider_changed =
        !first_read &&
        approximately_changed(panel.slider_a_percent, last_panel.slider_a_percent);

    if (buttons_changed)
    {
        apply_panel_mode(panel);

        if (!panel.master_switch_on)
        {
            upstream_state.stop_active = true;
            upstream_state.mode = "SHUTDOWN_REQUESTED";
            upstream_state.pwm = 0;
        }

        update_control_panel_leds(
            upstream_state.stop_active,
            upstream_state.slow_active,
            upstream_state.fast_active
        );
    }

    if (target_controlled_by_ui)
    {
        bool physical_slider_moved =
            abs(panel.slider_a_percent - slider_value_when_ui_took_control)
            > PHYSICAL_SLIDER_TAKEOVER_DELTA;

        if (physical_slider_moved)
        {
            target_controlled_by_ui = false;
            upstream_state.target_position = panel.slider_a_percent;
        }
    }
    else if (slider_changed)
    {
        upstream_state.target_position = panel.slider_a_percent;
    }

    last_panel = panel;
    has_last_panel = true;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    delay(1000);

    pins_init();
    control_panel_init();

    ControlPanelState initial_panel = read_control_panel();

    upstream_state.mode = "IDLE";
    upstream_state.direction = "STOP";
    upstream_state.stop_active = true;
    upstream_state.target_position = initial_panel.slider_a_percent;
    upstream_state.actual_position = 0.0f;

    last_panel = initial_panel;
    has_last_panel = true;

    Serial.println("READY");
}

void loop()
{
    if (downstream_available())
    {
        DownstreamCommand command = downstream_read();
        handle_downstream_command(command);
    }

    update_from_control_panel();

    static unsigned long last_upstream = 0;

    if (millis() - last_upstream >= 100)
    {
        last_upstream = millis();
        upstream_send();
    }
}