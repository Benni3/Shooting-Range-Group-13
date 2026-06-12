#include <Arduino.h>
#include "../Pins/pins.h"
#include "slider.h"

static bool ui_slider_a_enabled = false;
static bool ui_slider_b_enabled = false;

static float ui_slider_a_value = 0.0f;
static float ui_slider_b_value = 0.0f;

static float clamp_percent(float value)
{
    if (value < 0.0f)
        return 0.0f;

    if (value > 100.0f)
        return 100.0f;

    return value;
}

int read_slider_a_raw()
{
    if (ui_slider_a_enabled)
    {
        return (int)(ui_slider_a_value * 1023.0f / 100.0f);
    }

    return analogRead(PIN_POTENTIOMETER_A);
}

int read_slider_b_raw()
{
    if (ui_slider_b_enabled)
    {
        return (int)(ui_slider_b_value * 1023.0f / 100.0f);
    }

    return analogRead(PIN_POTENTIOMETER_B);
}

float read_slider_a_percent()
{
    if (ui_slider_a_enabled)
    {
        return ui_slider_a_value;
    }

    return analogRead(PIN_POTENTIOMETER_A) * 100.0f / 1023.0f;
}

float read_slider_b_percent()
{
    if (ui_slider_b_enabled)
    {
        return ui_slider_b_value;
    }

    return analogRead(PIN_POTENTIOMETER_B) * 100.0f / 1023.0f;
}

void set_ui_slider_a_percent(float percent)
{
    ui_slider_a_value = clamp_percent(percent);
    ui_slider_a_enabled = true;
}

void set_ui_slider_b_percent(float percent)
{
    ui_slider_b_value = clamp_percent(percent);
    ui_slider_b_enabled = true;
}

bool ui_slider_a_active()
{
    return ui_slider_a_enabled;
}

bool ui_slider_b_active()
{
    return ui_slider_b_enabled;
}

void clear_ui_slider_a()
{
    ui_slider_a_enabled = false;
}

void clear_ui_slider_b()
{
    ui_slider_b_enabled = false;
}