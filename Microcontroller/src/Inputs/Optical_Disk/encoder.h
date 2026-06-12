#pragma once

void encoder_init();
void encoder_update();

long encoder_get_counts();

float encoder_get_horizontal_position();
float encoder_get_horizontal_speed();
float encoder_get_horizontal_acceleration();

float encoder_get_rotational_position();
float encoder_get_angular_speed();
float encoder_get_angular_acceleration();