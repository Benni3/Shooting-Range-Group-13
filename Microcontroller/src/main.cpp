#include <Arduino.h>

#include "handshake.h"

#include "Pins/pin.h"
#include "Inputs/Control_Panel/control_panel.h"
#include "Stream/downstream.h"
#include "Stream/upstream.h"

void handle_downstream_command(const DownstreamCommand& command)
{
    if (downstream_is_none(command))
    {
        return;
    }

    if (downstream_is_handshake(command))
    {
        Serial.println(get_project_id());
    }

    else if (downstream_is_ping(command))
    {
        Serial.println("PONG");
    }

    else if (downstream_is_led_on(command))
    {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("LED_ON_OK");
    }

    else if (downstream_is_led_off(command))
    {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("LED_OFF_OK");
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
        upstream_state.target_position = command.value;
        control_panel_apply_ui_position_percent(command.value);
    }

    else if (downstream_is_manual_pwm(command))
    {
        int pwm = constrain((int)command.value, 0, 255);

        upstream_state.pwm = pwm;
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
        // Later this should go to a modular length handler.
        Serial.print("LENGTH_SET:");
        Serial.println(command.value, 2);
    }

    else
    {
        Serial.print("UNKNOWN_COMMAND: ");
        Serial.println(command.raw);
    }
}

void update_from_control_panel()
{
    ControlPanelState panel = read_control_panel();

    if (panel.emergency_pressed)
    {
        upstream_state.emergency_active = true;
        upstream_state.fast_active = false;
        upstream_state.slow_active = false;
        upstream_state.stop_active = true;
        upstream_state.mode = "EMERGENCY_STOP";
        upstream_state.direction = "STOP";
        upstream_state.pwm = 0;
    }

    else if (!upstream_state.emergency_active)
    {
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

    if (!panel.master_switch_on)
    {
        upstream_state.stop_active = true;
        upstream_state.mode = "SHUTDOWN_REQUESTED";
        upstream_state.pwm = 0;
    }

    upstream_state.target_position = panel.slider_a_percent;

    update_control_panel_leds(
        upstream_state.stop_active,
        upstream_state.slow_active,
        upstream_state.fast_active
    );
}

void update_placeholder_sensors()
{
    static unsigned long last_update = 0;

    if (millis() - last_update < 100)
    {
        return;
    }

    last_update = millis();

    float previous_position = upstream_state.actual_position;

    float error =
        upstream_state.target_position -
        upstream_state.actual_position;

    upstream_state.actual_position += error * 0.08f;

    upstream_state.horizontal_speed =
        (upstream_state.actual_position - previous_position) * 10.0f;

    upstream_state.horizontal_acceleration = 0.0f;

    upstream_state.rotational_position =
        upstream_state.actual_position * 7.5f * 360.0f;

    upstream_state.angular_speed =
        upstream_state.horizontal_speed * 7.5f * 360.0f;

    upstream_state.angular_acceleration = 0.0f;

    upstream_state.voltage = 12.0f;
    upstream_state.current = upstream_state.pwm / 255.0f * 2.2f;
    upstream_state.motor_temperature = 25.0f + upstream_state.current * 3.0f;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    delay(1000);

    pins_init();
    control_panel_init();

    upstream_state.mode = "IDLE";
    upstream_state.direction = "STOP";
    upstream_state.stop_active = true;
    upstream_state.target_position = 0.0f;
    upstream_state.actual_position = 0.0f;

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
    update_placeholder_sensors();

    static unsigned long last_upstream = 0;

    if (millis() - last_upstream >= 100)
    {
        last_upstream = millis();
        upstream_send();
    }
}