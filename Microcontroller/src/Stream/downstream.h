#pragma once

#include <Arduino.h>

enum class DownstreamCommandType
{
    NONE,

    HANDSHAKE,
    PING,

    LED_ON,
    LED_OFF,

    SET_MODE_FAST,
    SET_MODE_SLOW,

    EMERGENCY_STOP,
    CLEAR_EMERGENCY_STOP,

    SET_TARGET_POSITION,

    MANUAL_MOTOR_PWM,
    MANUAL_MOTOR_FORWARD,
    MANUAL_MOTOR_BACKWARD,
    MANUAL_MOTOR_STOP,

    RECALIBRATE_ZERO,
    SET_LENGTH
};

struct DownstreamCommand
{
    DownstreamCommandType type = DownstreamCommandType::NONE;
    float value = 0.0f;
    bool has_value = false;
    String raw = "";
};

bool downstream_available();
DownstreamCommand downstream_read();

bool downstream_is_none(const DownstreamCommand& command);

bool downstream_is_handshake(const DownstreamCommand& command);
bool downstream_is_ping(const DownstreamCommand& command);

bool downstream_is_led_on(const DownstreamCommand& command);
bool downstream_is_led_off(const DownstreamCommand& command);

bool downstream_is_fast(const DownstreamCommand& command);
bool downstream_is_slow(const DownstreamCommand& command);

bool downstream_is_emergency_stop(const DownstreamCommand& command);
bool downstream_is_clear_emergency_stop(const DownstreamCommand& command);

bool downstream_is_position(const DownstreamCommand& command);

bool downstream_is_manual_pwm(const DownstreamCommand& command);
bool downstream_is_manual_forward(const DownstreamCommand& command);
bool downstream_is_manual_backward(const DownstreamCommand& command);
bool downstream_is_manual_stop(const DownstreamCommand& command);

bool downstream_is_recalibrate_zero(const DownstreamCommand& command);
bool downstream_is_set_length(const DownstreamCommand& command);