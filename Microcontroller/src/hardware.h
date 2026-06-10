#pragma once

void hardware_init();

float read_position();
float read_current();
float read_voltage();

bool left_limit();
bool right_limit();
bool emergency_stop();

void motor_set_pwm(int pwm);
void motor_forward();
void motor_backward();
void motor_stop();

void led_fast(bool state);
void led_slow(bool state);
void led_stop(bool state);
void led_forward(bool state);
void led_back(bool state);
void led_emergency(bool state);