#include <Arduino.h>
#include "actuators.h"

// TB6612
#define AIN1 13
#define AIN2 14
#define PWMA 26

#define BIN1 12
#define BIN2 27
#define PWMB 25

#define MOTOR_STBY 33

// MOSFET
#define RELAY_PIN 15

#define POWER_PUMP 255
#define POWER_FAN 255

void init_actuators() {
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);

    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    pinMode(MOTOR_STBY, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(MOTOR_STBY, HIGH);

    rule_pump(false);
    rule_fan(false);
    rule_led(false);
}

void rule_pump(bool is_on) {
    digitalWrite(BIN1, is_on ? HIGH : LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, is_on ? POWER_PUMP : 0);
}

void rule_fan(bool is_on) {
  digitalWrite(AIN1, is_on ? HIGH : LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, is_on ? POWER_FAN : 0);
}


void rule_led(bool is_on) {
    digitalWrite(RELAY_PIN, is_on ? HIGH : LOW);
}