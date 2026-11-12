#ifndef ESP_SENSORS_H
#define ESP_SENSORS_H

// Пины датчиков
#define SOIL_SENSOR_PIN 34
#define WATER_SENSOR_PIN 35

// Калибровка влажности почвы
#define SOIL_DRY 3080   // Значение в сухой почве
#define SOIL_WET 800   // Значение в мокрой почве

// Калибровка уровня воды
#define WATER_EMPTY 0  // Значение при пустом резервуаре
#define WATER_FULL 4095  // Значение при полном резервуаре

void init_sensors();

float read_temperature();
float read_air_humidity();
int read_humidity_soil();
int read_water_level();

#endif