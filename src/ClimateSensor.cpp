#include <Arduino.h>
#include "ClimateSensor.h"
#include "common.h"

#ifndef SENSOR_UNIQUE_NAME
#define SENSOR_UNIQUE_NAME "example-name"
#endif

#define CLIMATE_STATE_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-climate/state"
#define STATE_DATA_FORMAT "{ \"temp\": %.2f, \"humid\": %.2f }"

#define TEMPERATURE_CONFIG_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-temp/config"
#define TEMPERATURE_CONFIG_DATA \
"{ " \
"\"name\": \"Temperature\", " \
"\"dev_cla\": \"temperature\", " \
"\"stat_t\": \"" CLIMATE_STATE_TOPIC "\", " \
"\"unit_of_meas\": \"°F\", " \
"\"val_tpl\": \"{{ value_json.temp }}\" " \
"}"

#define HUMIDITY_CONFIG_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-humid/config"
#define HUMIDITY_CONFIG_DATA \
"{ " \
"\"name\": \"Humidity\", " \
"\"dev_cla\": \"humidity\", " \
"\"stat_t\": \"" CLIMATE_STATE_TOPIC "\", " \
"\"unit_of_meas\": \"%\", " \
"\"val_tpl\": \"{{ value_json.humid }}\" " \
"}"

ClimateSensor::ClimateSensor(Scheduler* aScheduler, PubSubClient* mqttClient, uint8_t pin, uint8_t dhtType)
    : MqttSensor(aScheduler, mqttClient), tempSensor(pin, dhtType)
{
}

void ClimateSensor::Init()
{
    this->tempSensor.begin();
}

uint32_t ClimateSensor::OnMqttConnected()
{
    this->PublishMessage(TEMPERATURE_CONFIG_TOPIC, TEMPERATURE_CONFIG_DATA);
    this->PublishMessage(HUMIDITY_CONFIG_TOPIC, HUMIDITY_CONFIG_DATA);

    return 5 * TASK_MINUTE;
}

void ClimateSensor::ReadAndPublish()
{
    float temp = this->tempSensor.readTemperature(true);
    float humidity = this->tempSensor.readHumidity();

    if (isinf(temp) || isnan(temp) || isinf(humidity) || isnan(humidity))
    {
        DEBUG_TIME();
        DEBUG_SERIAL.print(temp);
        DEBUG_SERIAL.print(" °F, ");
        DEBUG_SERIAL.print(humidity);
        DEBUG_SERIAL.println(" %");
        return;
    }

    char payload[40];
    snprintf_P(payload, sizeof(payload), STATE_DATA_FORMAT, temp, humidity);

    this->PublishMessage(CLIMATE_STATE_TOPIC, payload);
}