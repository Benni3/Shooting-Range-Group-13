#include <Arduino.h>
#include "../../Pins/pins.h"
#include "LED.h"

void led_stop(bool on)
{
    digitalWrite(PIN_LED_STOP, on ? HIGH : LOW);
}

void led_slow(bool on)
{
    digitalWrite(PIN_LED_SLOW, on ? HIGH : LOW);
}

void led_fast(bool on)
{
    digitalWrite(PIN_LED_FAST, on ? HIGH : LOW);
}

void led_all_off()
{
    led_stop(false);
    led_slow(false);
    led_fast(false);
}

void led_show_stop()
{
    led_all_off();
    led_stop(true);
}

void led_show_slow()
{
    led_all_off();
    led_slow(true);
}

void led_show_fast()
{
    led_all_off();
    led_fast(true);
}