#pragma once

int read_slider_a_raw();
int read_slider_b_raw();

float read_slider_a_percent();
float read_slider_b_percent();

void set_ui_slider_a_percent(float percent);
void set_ui_slider_b_percent(float percent);

bool ui_slider_a_active();
bool ui_slider_b_active();

void clear_ui_slider_a();
void clear_ui_slider_b();