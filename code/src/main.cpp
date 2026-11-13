#include <Arduino.h>
#include "logic.h"
#include "esp_sensors.h"
#include "actuators.h"
#include "serial_control.h"

unsigned long last_sensor_read = 0;
const unsigned long SENSOR_INTERVAL = 2000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 GROWBOX STARTED!");

  // Инициализация железа
  init_sensors();
  init_actuators();

  // Связываем датчик температуры и влажности (SHT41 измеряет оба параметра)
  tmp.linked_sensor = &hmt;

  // Применяем начальный режим (день)
  tmp.set_range(day_mode.tmpr);
  hmt.set_range(day_mode.hmtr);
  shmt.set_range(day_mode.shmtr);
  wtl.set_range(day_mode.wtlr);

  Serial.println("GrowBox initialized. Type HELP for commands.");
}

void loop() {
  // 1. Обработка команд из Serial (вынесено в serial_control.cpp)
  handle_serial_command();

  // 2. Периодическое чтение датчиков и управление актуаторами
  if (millis() - last_sensor_read >= SENSOR_INTERVAL) {
    last_sensor_read = millis();

    // Чтение датчиков
    tmp.read_value();
    wtl.read_value();
    shmt.read_value();

    // Проверка алармов
    tmpMsg.controlling(s);
    wtlMsg.controlling(s);
    hmtMsg.controlling(s);

    // Управление актуаторами
    pump.controlling(s);
    vent.controlling(s);

    // Управление лампой по режиму день/ночь
    if (is_day_mode && !lamp.is_working()) {
      lamp.pin_turn_on();
    } else if (!is_day_mode && lamp.is_working()) {
      lamp.pin_turn_off();
    }

    // Периодическая отправка состояния
    msgmanager.controlling(s);

  }
}