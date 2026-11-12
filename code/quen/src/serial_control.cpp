#include "serial_control.h"
#include "logic.h"

// Флаг режима дня/ночи
bool is_day_mode = true;

// Вспомогательная функция для применения режима к датчикам
static void apply_mode_to_sensors() {
  if (is_day_mode) {
    tmp.set_range(day_mode.tmpr);
    hmt.set_range(day_mode.hmtr);
    shmt.set_range(day_mode.shmtr);
    wtl.set_range(day_mode.wtlr);
  } else {
    tmp.set_range(night_mode.tmpr);
    hmt.set_range(night_mode.hmtr);
    shmt.set_range(night_mode.shmtr);
    wtl.set_range(night_mode.wtlr);
  }
}

// Обработка входящих команд из Serial
void handle_serial_command() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd.startsWith("SET_T_MIN ")) {
    day_mode.tmpr.min = cmd.substring(10).toFloat();
    if (is_day_mode) tmp.set_range(day_mode.tmpr);
    Serial.println("OK: T_MIN set to " + String(day_mode.tmpr.min));
  }
  else if (cmd.startsWith("SET_T_MAX ")) {
    day_mode.tmpr.max = cmd.substring(10).toFloat();
    if (is_day_mode) tmp.set_range(day_mode.tmpr);
    Serial.println("OK: T_MAX set to " + String(day_mode.tmpr.max));
  }
  else if (cmd.startsWith("SET_H_MIN ")) {
    day_mode.hmtr.min = cmd.substring(10).toFloat();
    if (is_day_mode) hmt.set_range(day_mode.hmtr);
    Serial.println("OK: H_MIN set to " + String(day_mode.hmtr.min));
  }
  else if (cmd.startsWith("SET_H_MAX ")) {
    day_mode.hmtr.max = cmd.substring(10).toFloat();
    if (is_day_mode) hmt.set_range(day_mode.hmtr);
    Serial.println("OK: H_MAX set to " + String(day_mode.hmtr.max));
  }
  else if (cmd.startsWith("SET_S_MIN ")) {
    day_mode.shmtr.min = cmd.substring(10).toFloat();
    if (is_day_mode) shmt.set_range(day_mode.shmtr);
    Serial.println("OK: S_MIN set to " + String(day_mode.shmtr.min));
  }
  else if (cmd.startsWith("SET_S_MAX ")) {
    day_mode.shmtr.max = cmd.substring(10).toFloat();
    if (is_day_mode) shmt.set_range(day_mode.shmtr);
    Serial.println("OK: S_MAX set to " + String(day_mode.shmtr.max));
  }
  else if (cmd.startsWith("SET_W_MIN ")) {
    day_mode.wtlr.min = cmd.substring(10).toFloat();
    if (is_day_mode) wtl.set_range(day_mode.wtlr);
    Serial.println("OK: W_MIN set to " + String(day_mode.wtlr.min));
  }
  else if (cmd.startsWith("SET_W_MAX ")) {
    day_mode.wtlr.max = cmd.substring(10).toFloat();
    if (is_day_mode) wtl.set_range(day_mode.wtlr);
    Serial.println("OK: W_MAX set to " + String(day_mode.wtlr.max));
  }
  else if (cmd == "MODE DAY") {
    is_day_mode = true;
    apply_mode_to_sensors();
    Serial.println("OK: Switched to DAY mode");
  }
  else if (cmd == "MODE NIGHT") {
    is_day_mode = false;
    apply_mode_to_sensors();
    Serial.println("OK: Switched to NIGHT mode");
  }
  else if (cmd == "STATUS") {
    Serial.println("--- CURRENT STATUS ---");
    Serial.println("Mode: " + String(is_day_mode ? "DAY" : "NIGHT"));
    Serial.println("Temp Range: " + String(tmp.range.min) + " - " + String(tmp.range.max));
    Serial.println("Air Hum Range: " + String(hmt.range.min) + " - " + String(hmt.range.max));
    Serial.println("Soil Hum Range: " + String(shmt.range.min) + " - " + String(shmt.range.max));
    Serial.println("Water Range: " + String(wtl.range.min) + " - " + String(wtl.range.max));
    Serial.println("Fan: " + String(vent.is_working() ? "ON" : "OFF"));
    Serial.println("Pump: " + String(pump.is_working() ? "ON" : "OFF"));
    Serial.println("LED: " + String(lamp.is_working() ? "ON" : "OFF"));
    Serial.println("----------------------");
  }
  else if (cmd == "HELP") {
    Serial.println("Commands:");
    Serial.println("  SET_T_MIN <val>, SET_T_MAX <val>");
    Serial.println("  SET_H_MIN <val>, SET_H_MAX <val>");
    Serial.println("  SET_S_MIN <val>, SET_S_MAX <val>");
    Serial.println("  SET_W_MIN <val>, SET_W_MAX <val>");
    Serial.println("  MODE DAY, MODE NIGHT");
    Serial.println("  STATUS, HELP");
  }
  else {
    Serial.println("ERR: Unknown command. Type HELP");
  }
}