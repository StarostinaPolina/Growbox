#ifndef LOGIC_H
#define LOGIC_H

#include <Arduino.h>

typedef struct {
  float min;
  float max;
} Range;

enum Logic_value { LOGIC_LOW, NORM, LOGIC_HIGH };

class CSensor {
public:
  Range range;
  float value;
  Logic_value logic_value;
  int pin;
  String sensor_name;
  bool user_control = false;

  virtual void read_sensor_value() = 0;
  virtual void set_logic_value();
  Logic_value get_lv() { return logic_value; }
  virtual void read_value();
  void set_value(float v);
  void set_range(Range r);
  
  CSensor(String name, int p, Range r);
  String sensor_to_string();
};

class CTempSensor : public CSensor {
public:
  using CSensor::CSensor;
  CSensor* linked_sensor;
  void read_sensor_value() override;
};

class CHumiditySensor : public CSensor {
public:
  using CSensor::CSensor;
  void read_sensor_value() override { return; } // Значение берется из CTempSensor
};

class CSoilHumidity : public CSensor {
public:
  using CSensor::CSensor;
  void read_sensor_value() override;
};

class CWaterLevel : public CSensor {
public:
  using CSensor::CSensor;
  void read_sensor_value() override;
};

typedef struct {
  CSensor* t;
  CSensor* h_air;
  CSensor* h_ground;
  CSensor* water_level;
} Sensors;

class CActuator {
public:
  int pin = 0;
  unsigned long worktime = 0;
  unsigned long sleeptime = 0;
  unsigned long PLAN_WORKTIME = 0;
  bool isWorking = false;
  String name;

  CActuator(String name, unsigned long pwt, int p);
  bool is_working();
  virtual void pin_turn_on() = 0;
  virtual void pin_turn_off() = 0;
  virtual void start(Sensors s);
  virtual bool uniq_start(Sensors s) = 0;
  virtual void stop(Sensors s);
  virtual bool uniq_stop(Sensors s) { return false; }
  void change_timers();
  virtual void controlling(Sensors s);
};

class CVent : public CActuator {
public:
  using CActuator::CActuator;
  void pin_turn_on() override;
  void pin_turn_off() override;
  bool uniq_start(Sensors s) override;
};

class CManagerMsg : public CActuator {
public:
  Sensors* sensors;
  bool alarm_msg = false;
  CManagerMsg(Sensors* s, String name, unsigned long t, int pin);
  void pin_turn_on() override;
  void pin_turn_off() override { return; }
  bool uniq_start(Sensors s) override;
  void controlling(Sensors s) override;
};

class CAlarmMsg : public CActuator {
public:
  CSensor* s;
  CManagerMsg* manager;
  CAlarmMsg(String name, CSensor* sensor, unsigned long t, CManagerMsg* m);
  void pin_turn_on() override;
  void pin_turn_off() override;
  bool uniq_start(Sensors s) override;
  bool uniq_stop(Sensors s) override;
};

class CPump : public CActuator {
public:
  using CActuator::CActuator;
  void pin_turn_on() override;
  void pin_turn_off() override;
  bool uniq_start(Sensors snrs) override;
};

class CLamp : public CActuator {
public:
  unsigned long start_time;
  unsigned long end_time;
  using CActuator::CActuator;
  void pin_turn_on() override;
  void pin_turn_off() override;
  bool uniq_start(Sensors snrs) override;
};

struct parameters {
  Range tmpr, hmtr, shmtr, wtlr;
  unsigned long vent_worktime;
  unsigned long pump_worktime;
  unsigned long alarm_msg_repeate_time;
  unsigned long msg_repeate_time;
  unsigned long start_day_mode;
  unsigned long start_night_mode;
};

extern parameters day_mode;
extern parameters night_mode;
extern CTempSensor tmp;
extern CHumiditySensor hmt;
extern CSoilHumidity shmt;
extern CWaterLevel wtl;
extern Sensors s;
extern CPump pump;
extern CVent vent;
extern CLamp lamp;
extern CManagerMsg msgmanager;
extern CAlarmMsg wtlMsg;
extern CAlarmMsg tmpMsg;
extern CAlarmMsg hmtMsg;

void print_debug(String s);
void tick_timers(); // Функция для уменьшения таймеров каждый такт (например, раз в секунду)

#endif