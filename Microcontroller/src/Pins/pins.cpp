#include <Arduino.h>
#include "pins.h"

void pins_init()
{
    // =========================
    // INPUTS
    // =========================

    pinMode(PIN_POTENTIOMETER_A, INPUT);
    pinMode(PIN_POTENTIOMETER_B, INPUT);

    pinMode(PIN_CURRENT_SENSOR, INPUT);
    pinMode(PIN_VOLTAGE_SENSOR, INPUT);
    pinMode(PIN_TEMPERATURE_SENSOR, INPUT);

    pinMode(PIN_ENCODER_1, INPUT_PULLUP);
    pinMode(PIN_ENCODER_2, INPUT_PULLUP);

    // =========================
    // OUTPUTS
    // =========================

    pinMode(PIN_LED_STOP, OUTPUT);
    pinMode(PIN_LED_SLOW, OUTPUT);
    pinMode(PIN_LED_FAST, OUTPUT);

    // Default state
    digitalWrite(PIN_LED_STOP, LOW);
    digitalWrite(PIN_LED_SLOW, LOW);
    digitalWrite(PIN_LED_FAST, LOW);
}