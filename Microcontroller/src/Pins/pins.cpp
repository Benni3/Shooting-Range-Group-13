#include <Arduino.h>
#include "pins.h"

void pins_init()
{
    // =====================================
    // CONTROL PANEL INPUTS
    // =====================================

    pinMode(PIN_POTENTIOMETER_A, INPUT);
    pinMode(PIN_POTENTIOMETER_B, INPUT);

    pinMode(SLOW_BUTTON, INPUT_PULLUP);
    pinMode(FAST_BUTTON, INPUT_PULLUP);
    pinMode(EMERGENCY_STOP_BUTTON, INPUT_PULLUP);

    pinMode(SWITCH, INPUT_PULLUP);

    // =====================================
    // MOTOR SAFETY INPUTS
    // =====================================

    pinMode(PIN_CURRENT_SENSOR, INPUT);
    pinMode(PIN_VOLTAGE_SENSOR, INPUT);
    pinMode(PIN_TEMPERATURE_SENSOR, INPUT);

    // =====================================
    // ENCODER INPUTS
    // =====================================

    pinMode(PIN_ENCODER_1, INPUT_PULLUP);
    pinMode(PIN_ENCODER_2, INPUT_PULLUP);

    // =====================================
    // STATUS LED OUTPUTS
    // =====================================

    pinMode(PIN_LED_STOP, OUTPUT);
    pinMode(PIN_LED_SLOW, OUTPUT);
    pinMode(PIN_LED_FAST, OUTPUT);

    // =====================================
    // DEFAULT STATES
    // =====================================

    digitalWrite(PIN_LED_STOP, LOW);
    digitalWrite(PIN_LED_SLOW, LOW);
    digitalWrite(PIN_LED_FAST, LOW);
}