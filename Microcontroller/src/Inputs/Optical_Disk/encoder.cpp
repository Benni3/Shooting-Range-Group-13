#include <Arduino.h>
#include "../Pins/pins.h"
#include "encoder.h"

static long counts = 0;

static int last_a = 0;
static int last_b = 0;

static unsigned long last_time_ms = 0;

static long last_counts = 0;

static float horizontal_position = 0.0f;
static float horizontal_speed = 0.0f;
static float horizontal_acceleration = 0.0f;
static float last_horizontal_speed = 0.0f;

static float rotational_position = 0.0f;
static float angular_speed = 0.0f;
static float angular_acceleration = 0.0f;
static float last_angular_speed = 0.0f;

// You measured: 7.5 revolutions = 1 cm
static const float REV_PER_CM = 7.5f;

// With two optical sensors as quadrature, start with 4 counts per revolution.
// If position is wrong, calibrate this later.
static const float COUNTS_PER_REV = 4.0f;

void encoder_init()
{
    pinMode(PIN_ENCODER_1, INPUT_PULLUP);
    pinMode(PIN_ENCODER_2, INPUT_PULLUP);

    last_a = digitalRead(PIN_ENCODER_1);
    last_b = digitalRead(PIN_ENCODER_2);

    last_time_ms = millis();
}

void encoder_update()
{
    int a = digitalRead(PIN_ENCODER_1);
    int b = digitalRead(PIN_ENCODER_2);

    if (a != last_a)
    {
        if (a == b)
            counts++;
        else
            counts--;
    }

    last_a = a;
    last_b = b;

    unsigned long now = millis();
    float dt = (now - last_time_ms) / 1000.0f;

    if (dt < 0.05f)
        return;

    long delta_counts = counts - last_counts;

    float revolutions = counts / COUNTS_PER_REV;
    float delta_revolutions = delta_counts / COUNTS_PER_REV;

    rotational_position = revolutions * 360.0f;
    horizontal_position = revolutions / REV_PER_CM;

    angular_speed = delta_revolutions * 360.0f / dt;
    horizontal_speed = delta_revolutions / REV_PER_CM / dt;

    angular_acceleration = (angular_speed - last_angular_speed) / dt;
    horizontal_acceleration = (horizontal_speed - last_horizontal_speed) / dt;

    last_angular_speed = angular_speed;
    last_horizontal_speed = horizontal_speed;

    last_counts = counts;
    last_time_ms = now;
}

long encoder_get_counts()
{
    return counts;
}

float encoder_get_horizontal_position()
{
    return horizontal_position;
}

float encoder_get_horizontal_speed()
{
    return horizontal_speed;
}

float encoder_get_horizontal_acceleration()
{
    return horizontal_acceleration;
}

float encoder_get_rotational_position()
{
    return rotational_position;
}

float encoder_get_angular_speed()
{
    return angular_speed;
}

float encoder_get_angular_acceleration()
{
    return angular_acceleration;
}