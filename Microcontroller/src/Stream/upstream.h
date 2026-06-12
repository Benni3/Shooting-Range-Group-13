#pragma once

struct UpstreamState
{
    // =========================
    // Position
    // =========================

    float target_position = 0.0f;
    float actual_position = 0.0f;

    float horizontal_speed = 0.0f;
    float horizontal_acceleration = 0.0f;

    // =========================
    // Rotation
    // =========================

    float rotational_position = 0.0f;
    float angular_speed = 0.0f;
    float angular_acceleration = 0.0f;

    // =========================
    // Motor
    // =========================

    float voltage = 0.0f;
    float current = 0.0f;
    float motor_temperature = 0.0f;

    int pwm = 0;

    // =========================
    // State
    // =========================

    bool stop_active = false;
    bool slow_active = false;
    bool fast_active = false;

    bool forward_active = false;
    bool back_active = false;

    bool emergency_active = false;

    // =========================
    // Strings for UI
    // =========================

    const char* mode = "IDLE";
    const char* direction = "STOP";
};

extern UpstreamState upstream_state;

void upstream_send();