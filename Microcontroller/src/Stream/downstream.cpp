#include <Arduino.h>
#include "downstream.h"

static String read_serial_line()
{
    String line = Serial.readStringUntil('\n');
    line.trim();
    return line;
}

static float parse_value_after_colon(const String& line)
{
    int index = line.indexOf(':');

    if (index < 0)
    {
        return 0.0f;
    }

    String value = line.substring(index + 1);
    value.trim();

    return value.toFloat();
}

bool downstream_available()
{
    return Serial.available() > 0;
}

DownstreamCommand downstream_read()
{
    DownstreamCommand command;

    if (!downstream_available())
    {
        return command;
    }

    String line = read_serial_line();
    command.raw = line;

    if (line.length() == 0)
    {
        return command;
    }

    // =========================
    // BASIC CONNECTION COMMANDS
    // =========================

    if (line == "WHO_ARE_YOU?" || line == "HANDSHAKE")
    {
        command.type = DownstreamCommandType::HANDSHAKE;
        return command;
    }

    if (line == "PING")
    {
        command.type = DownstreamCommandType::PING;
        return command;
    }

    // =========================
    // DEBUG LED COMMANDS
    // =========================

    if (line == "LED_ON")
    {
        command.type = DownstreamCommandType::LED_ON;
        return command;
    }

    if (line == "LED_OFF")
    {
        command.type = DownstreamCommandType::LED_OFF;
        return command;
    }

    // =========================
    // MODE COMMANDS
    // =========================

    if (line == "FAST" || line == "SET_MODE:FAST")
    {
        command.type = DownstreamCommandType::SET_MODE_FAST;
        return command;
    }

    if (line == "SLOW" || line == "SET_MODE:SLOW")
    {
        command.type = DownstreamCommandType::SET_MODE_SLOW;
        return command;
    }

    // =========================
    // EMERGENCY COMMANDS
    // =========================

    if (line == "EMERGENCY_STOP")
    {
        command.type = DownstreamCommandType::EMERGENCY_STOP;
        return command;
    }

    if (line == "CLEAR_EMERGENCY_STOP")
    {
        command.type = DownstreamCommandType::CLEAR_EMERGENCY_STOP;
        return command;
    }

    // =========================
    // POSITION COMMANDS
    // =========================

    if (line.startsWith("SET_TARGET_POSITION:"))
    {
        command.type = DownstreamCommandType::SET_TARGET_POSITION;
        command.value = parse_value_after_colon(line);
        command.has_value = true;
        return command;
    }

    if (line.startsWith("SET_POSITION:"))
    {
        command.type = DownstreamCommandType::SET_TARGET_POSITION;
        command.value = parse_value_after_colon(line);
        command.has_value = true;
        return command;
    }

    // =========================
    // MANUAL MOTOR COMMANDS
    // =========================

    if (line.startsWith("SET_PWM:"))
    {
        command.type = DownstreamCommandType::MANUAL_MOTOR_PWM;
        command.value = parse_value_after_colon(line);
        command.has_value = true;
        return command;
    }

    if (line == "SET_DIRECTION:FORWARD" || line == "MOTOR_FORWARD")
    {
        command.type = DownstreamCommandType::MANUAL_MOTOR_FORWARD;
        return command;
    }

    if (
        line == "SET_DIRECTION:BACK" ||
        line == "SET_DIRECTION:BACKWARD" ||
        line == "MOTOR_BACKWARD"
    )
    {
        command.type = DownstreamCommandType::MANUAL_MOTOR_BACKWARD;
        return command;
    }

    if (line == "SET_DIRECTION:STOP" || line == "MOTOR_STOP")
    {
        command.type = DownstreamCommandType::MANUAL_MOTOR_STOP;
        return command;
    }

    // =========================
    // CALIBRATION / CONFIG
    // =========================

    if (line == "RECALIBRATE_ZERO" || line == "RECALIBRATE_POSITION_ZERO")
    {
        command.type = DownstreamCommandType::RECALIBRATE_ZERO;
        return command;
    }

    if (line.startsWith("SET_LENGTH:"))
    {
        command.type = DownstreamCommandType::SET_LENGTH;
        command.value = parse_value_after_colon(line);
        command.has_value = true;
        return command;
    }

    return command;
}

bool downstream_is_none(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::NONE;
}

bool downstream_is_handshake(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::HANDSHAKE;
}

bool downstream_is_ping(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::PING;
}

bool downstream_is_led_on(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::LED_ON;
}

bool downstream_is_led_off(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::LED_OFF;
}

bool downstream_is_fast(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::SET_MODE_FAST;
}

bool downstream_is_slow(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::SET_MODE_SLOW;
}

bool downstream_is_emergency_stop(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::EMERGENCY_STOP;
}

bool downstream_is_clear_emergency_stop(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::CLEAR_EMERGENCY_STOP;
}

bool downstream_is_position(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::SET_TARGET_POSITION;
}

bool downstream_is_manual_pwm(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::MANUAL_MOTOR_PWM;
}

bool downstream_is_manual_forward(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::MANUAL_MOTOR_FORWARD;
}

bool downstream_is_manual_backward(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::MANUAL_MOTOR_BACKWARD;
}

bool downstream_is_manual_stop(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::MANUAL_MOTOR_STOP;
}

bool downstream_is_recalibrate_zero(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::RECALIBRATE_ZERO;
}

bool downstream_is_set_length(const DownstreamCommand& command)
{
    return command.type == DownstreamCommandType::SET_LENGTH;
}