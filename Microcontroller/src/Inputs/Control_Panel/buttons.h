#pragma once

bool slow_button_pressed();
bool fast_button_pressed();
bool emergency_button_pressed();
bool switch_enabled();

void set_ui_slow_button(bool pressed);
void set_ui_fast_button(bool pressed);
void set_ui_emergency_button(bool pressed);
void set_ui_switch(bool enabled);