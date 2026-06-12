#include <Arduino.h>
#include "upstream.h"

UpstreamState upstream_state;

void upstream_send()
{
    Serial.print("POS=");
    Serial.print(upstream_state.actual_position, 2);

    Serial.print(",TARGET=");
    Serial.print(upstream_state.target_position, 2);

    Serial.print(",HSPD=");
    Serial.print(upstream_state.horizontal_speed, 2);

    Serial.print(",HACC=");
    Serial.print(upstream_state.horizontal_acceleration, 2);

    Serial.print(",RPOS=");
    Serial.print(upstream_state.rotational_position, 2);

    Serial.print(",ASPD=");
    Serial.print(upstream_state.angular_speed, 2);

    Serial.print(",RACC=");
    Serial.print(upstream_state.angular_acceleration, 2);

    Serial.print(",VOLT=");
    Serial.print(upstream_state.voltage, 2);

    Serial.print(",CUR=");
    Serial.print(upstream_state.current, 2);

    Serial.print(",TEMP=");
    Serial.print(upstream_state.motor_temperature, 2);

    Serial.print(",PWM=");
    Serial.print(upstream_state.pwm);

    Serial.print(",MODE=");
    Serial.print(upstream_state.mode);

    Serial.print(",DIR=");
    Serial.print(upstream_state.direction);

    Serial.print(",STOP=");
    Serial.print(upstream_state.stop_active ? 1 : 0);

    Serial.print(",SLOW=");
    Serial.print(upstream_state.slow_active ? 1 : 0);

    Serial.print(",FAST=");
    Serial.print(upstream_state.fast_active ? 1 : 0);

    Serial.print(",FWD=");
    Serial.print(upstream_state.forward_active ? 1 : 0);

    Serial.print(",BACK=");
    Serial.print(upstream_state.back_active ? 1 : 0);

    Serial.print(",EMG=");
    Serial.print(upstream_state.emergency_active ? 1 : 0);

    Serial.println();
}