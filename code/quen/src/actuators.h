#ifndef ACTUATORS_H
#define ACTUATORS_H

void init_actuators();

void rule_pump(bool is_on);
void rule_fan(bool is_on);
void rule_led(bool is_on);

#endif