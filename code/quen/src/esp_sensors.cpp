#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT4x.h>

#include "esp_sensors.h"


Adafruit_SHT4x sht4;

void init_sensors() {
    Wire.begin(21, 22);

    if (!sht4.begin()) {
        Serial.println("SHT41 not found!");
    }
}

float read_temperature() {
    sensors_event_t humidity, temperature;
    sht4.getEvent(&humidity, &temperature);

    return temperature.temperature;
}

float read_air_humidity() {
    sensors_event_t humidity, temperature;
    sht4.getEvent(&humidity, &temperature);

    return humidity.relative_humidity;
}

int read_humidity_soil() {
    
    int raw_value = analogRead(SOIL_SENSOR_PIN);
    int humidity = map(raw_value, SOIL_DRY, SOIL_WET, 0, 100);

    return constrain(humidity, 0, 100);
}

int read_water_level() {

    int raw_value = analogRead(WATER_SENSOR_PIN);
    int level = map(raw_value, WATER_EMPTY, WATER_FULL, 0, 100);

    return constrain(level, 0, 100);
}