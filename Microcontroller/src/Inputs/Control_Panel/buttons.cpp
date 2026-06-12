#include <Arduino.h>
#include "../../Pins/pins.h"
#include "buttons.h"

static bool ui_slow_pressed = false;
static bool ui_fast_pressed = false;
static bool ui_emergency_pressed = false;
static bool ui_master_switch_on = false;

bool slow_button_pressed()
{
    return digitalRead(SLOW_BUTTON) == LOW || ui_slow_pressed;
}

bool fast_button_pressed()
{
    return digitalRead(FAST_BUTTON) == LOW || ui_fast_pressed;
}

bool emergency_button_pressed()
{
    return digitalRead(EMERGENCY_STOP_BUTTON) == LOW || ui_emergency_pressed;
}

bool master_switch_on()
{
    return digitalRead(SWITCH) == LOW || ui_master_switch_on;
}

void set_ui_slow_button(bool pressed)
{
    ui_slow_pressed = pressed;
}

void set_ui_fast_button(bool pressed)
{
    ui_fast_pressed = pressed;
}

void set_ui_emergency_button(bool pressed)
{
    ui_emergency_pressed = pressed;
}

void set_ui_switch(bool enabled)
{
    ui_master_switch_on = enabled;
}