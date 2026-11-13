#ifndef SERIAL_CONTROL_H
#define SERIAL_CONTROL_H

#include <Arduino.h>

// Объявление функции обработки Serial-команд
void handle_serial_command();

// Флаг текущего режима (день/ночь) — определяется в serial_control.cpp,
// но используется также в main.cpp для управления лампой
extern bool is_day_mode;

#endif