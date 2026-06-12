#pragma once

#include <Arduino.h>

// ========= INPUTS =============

// Inputs from control panel
constexpr uint8_t PIN_POTENTIOMETER_A     = A4;
constexpr uint8_t PIN_POTENTIOMETER_B     = A5;
constexpr uint8_t SLOW_BUTTON             =  9;
constexpr uint8_t FAST_BUTTON             =  6;
constexpr uint8_t EMERGENCY_STOP_BUTTON   =  5;
constexpr uint8_t SWITCH                  =  4;


// Inputs for motor safety
constexpr uint8_t PIN_CURRENT_SENSOR       = A1;
constexpr uint8_t PIN_VOLTAGE_SENSOR       = A2;
constexpr uint8_t PIN_TEMPERATURE_SENSOR   = 2;

// Encoder inputs
constexpr uint8_t PIN_ENCODER_1            = 8;
constexpr uint8_t PIN_ENCODER_2            = 7;

// ========= OUTPUTS ============

// LED lights indicating state
constexpr uint8_t PIN_LED_STOP             = 10;
constexpr uint8_t PIN_LED_SLOW             = 11;
constexpr uint8_t PIN_LED_FAST             = 12;