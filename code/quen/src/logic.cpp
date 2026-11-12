#include "logic.h"
// Подключаем ваши файлы работы с железом
#include "esp_sensors.h" 
#include "actuators.h"

// --- CSensor ---
CSensor::CSensor(String name, int p, Range r) {
  value = 0; logic_value = LOGIC_LOW; pin = p; sensor_name = name; range = r;
}
void CSensor::set_logic_value() {
  if (value < range.min) logic_value = LOGIC_LOW;
  else if (value > range.max) logic_value = LOGIC_HIGH;
  else logic_value = NORM;
}
void CSensor::read_value() {
  read_sensor_value();
  set_logic_value();
  // print_debug("Read " + sensor_to_string());
}
void CSensor::set_value(float v) { value = v; set_logic_value(); }
void CSensor::set_range(Range r) { range = r; set_logic_value(); }

String CSensor::sensor_to_string() {
  return sensor_name + " val=" + String(value, 2) + " st=" + String(logic_value);
}

// --- Реализация чтения датчиков (Связь с esp_sensors.cpp) ---
void CTempSensor::read_sensor_value() {
  float t = read_temperature();
  float h = read_air_humidity();
  set_value(t);
  if (linked_sensor != NULL) linked_sensor->set_value(h);
}
void CSoilHumidity::read_sensor_value() { set_value(read_humidity_soil()); }
void CWaterLevel::read_sensor_value() { set_value(read_water_level()); }

// --- CActuator ---
CActuator::CActuator(String sname, unsigned long pwt, int p) {
  PLAN_WORKTIME = pwt; pin = p; name = sname;
}
bool CActuator::is_working() { return isWorking; }
void CActuator::start(Sensors s) {
  if ((worktime < 2) && (uniq_start(s))) {
    if (!isWorking) { pin_turn_on(); isWorking = true; }
  } else return;
  worktime += PLAN_WORKTIME; sleeptime = 0;
}
void CActuator::stop(Sensors s) {
  if (!is_working()) return;
  if (uniq_stop(s) || (worktime == 0)) {
    pin_turn_off(); worktime = 0; isWorking = false;
  }
}
void CActuator::change_timers() {
  if (worktime > 0) worktime--;
  else if (sleeptime > 0) sleeptime--;
}
void CActuator::controlling(Sensors s) { start(s); stop(s); change_timers(); }

// --- Актуаторы (Связь с actuators.cpp) ---
void CVent::pin_turn_on() { rule_fan(true); print_debug(name + " FAN ON"); }
void CVent::pin_turn_off() { rule_fan(false); print_debug(name + " FAN OFF"); }
bool CVent::uniq_start(Sensors s) { return (s.t->get_lv() == LOGIC_HIGH || s.h_air->get_lv() == LOGIC_HIGH); }

void CPump::pin_turn_on() { rule_pump(true); print_debug(name + " PUMP ON"); }
void CPump::pin_turn_off() { rule_pump(false); print_debug(name + " PUMP OFF"); }
bool CPump::uniq_start(Sensors snrs) { 
  return (snrs.h_ground->get_lv() == LOGIC_LOW) && (snrs.water_level->get_lv() != LOGIC_LOW); 
}

void CLamp::pin_turn_on() { rule_led(true);
    //print_debug(name + " LED ON");
    }
void CLamp::pin_turn_off() { rule_led(false); print_debug(name + " LED OFF"); }
bool CLamp::uniq_start(Sensors snrs) { return false; } // Логика дня/ночи реализуется в main

// --- Менеджер сообщений ---
CManagerMsg::CManagerMsg(Sensors* s, String name, unsigned long t, int pin) : CActuator(name, t, pin) { sensors = s; }


void CManagerMsg::pin_turn_on() {
  String msg1 = sensors->t->sensor_to_string() + (sensors->t->user_control ? " ALARM;   " : ";   ");
  msg1 += sensors->water_level->sensor_to_string() + (sensors->water_level->user_control ? " ALARM;   " : ";    ");
  msg1 += sensors->h_air->sensor_to_string() + (sensors->h_air->user_control ? " ALARM;    " : ";    ");

  msg1 += sensors->h_ground->sensor_to_string();
  print_debug("CURRENT SENSOR VALUES:");
  print_debug(msg1);
  //print_debug(msg2);
}
bool CManagerMsg::uniq_start(Sensors s) { return (worktime == 0 && !isWorking); }
void CManagerMsg::controlling(Sensors s) {
  if (alarm_msg) { pin_turn_on(); alarm_msg = false; } 
  else start(s);
  change_timers(); stop(s);
}

// --- Алармы ---
CAlarmMsg::CAlarmMsg(String name, CSensor* sensor, unsigned long t, CManagerMsg* m) : CActuator(name, t, 0) { s = sensor; manager = m; }
void CAlarmMsg::pin_turn_on() {
  print_debug("ALARM: " + name + " LOGIC_LOW " + s->sensor_to_string());
  manager->alarm_msg = true; s->user_control = true;
}
void CAlarmMsg::pin_turn_off() {
  print_debug("NORM: " + name + " " + s->sensor_to_string());
  manager->alarm_msg = true; s->user_control = false;
}
bool CAlarmMsg::uniq_start(Sensors snrs) { return (this->s->logic_value == LOGIC_LOW); }
bool CAlarmMsg::uniq_stop(Sensors snrs)  { return (this->s->logic_value != LOGIC_LOW); }

// --- Глобальные переменные и инициализация ---
parameters day_mode = {{18, 25}, {80, 100}, {50, 70}, {40, 100}, 5, 3, 4, 8, 0, 0};
parameters night_mode = {{16, 22}, {50, 70}, {50, 70}, {20, 100}, 5, 3, 4, 8, 0, 0};

CTempSensor tmp("SHT41_T", 0, day_mode.tmpr);
CHumiditySensor hmt("SHT41_H", 0, day_mode.hmtr);
CSoilHumidity shmt("SH", 0, day_mode.shmtr);
CWaterLevel wtl("WL", 0, day_mode.wtlr);
Sensors s = {&tmp, &hmt, &shmt, &wtl};

CPump pump("PUMP", 3, 0);
CVent vent("VENT", 5, 0);
CLamp lamp("LAMP", 0, 0);
CManagerMsg msgmanager(&s, "EventMgr", 10, 0);
CAlarmMsg wtlMsg("WLA", &wtl, 3, &msgmanager);
CAlarmMsg tmpMsg("TMPA", &tmp, 3, &msgmanager);
CAlarmMsg hmtMsg("HMA", &hmt, 3, &msgmanager);

void print_debug(String s) {
  Serial.println(s);
}

unsigned long last_tick = 0;
void tick_timers() {
  // Уменьшаем таймеры раз в 1 секунду (1000 мс)
  if (millis() - last_tick >= 1000) {
    last_tick = millis();
    // Здесь можно вызывать change_timers() для всех актуаторов, 
    // но в вашей логике это уже делается внутри controlling()
  }
}


//SET_H_MIN 20