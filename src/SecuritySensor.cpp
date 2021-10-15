#include <Arduino.h>
#include "common.h"
#include "SecuritySensor.h"

#ifndef SENSOR_UNIQUE_NAME
#define SENSOR_UNIQUE_NAME "example-name"
#endif

#define SECURITY_STATE_TOPIC "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "/state"

#if _SECURITY_ZONE_COUNT == 6
#define ZONE_DATA_FORMAT "{ \"z1\": \"%s\", \"z2\": \"%s\", \"z3\": \"%s\", \"z4\": \"%s\", \"z5\": \"%s\", \"z6\": \"%s\" }"
#endif

#define ZONE_CONFIG_TOPIC_FORMAT "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "-z%u/config"
#define ZONE_CONFIG_DATA_FORMAT \
"{ " \
"\"name\": \"Zone %u\", " \
"\"dev_cla\": \"opening\", " \
"\"stat_t\": \"" SECURITY_STATE_TOPIC "\", " \
"\"val_tpl\": \"{{ value_json.z%u }}\" " \
"}"

SecuritySensor::SecuritySensor(Scheduler* aScheduler, PubSubClient* mqttClient, pin_size_t* zone_pin_arry)
    : MqttSensor(aScheduler, mqttClient)
{
    for (uint8_t i = 0; i < _SECURITY_ZONE_COUNT; i++)
    {
        this->zone_pins[i] = zone_pin_arry[i];
    }
}

void SecuritySensor::Init()
{
    for (uint8_t i = 0; i < _SECURITY_ZONE_COUNT; i++)
    {
        pinMode(this->zone_pins[i], PinMode::INPUT);
    }
}

uint32_t SecuritySensor::OnMqttConnected()
{
    // %u will be replaced with an int that is almost certainly less than 100
    auto topic_len = strlen(ZONE_CONFIG_TOPIC_FORMAT) + 1;
    char topic[topic_len];

    auto data_len = strlen(ZONE_CONFIG_DATA_FORMAT) + 1;
    char data[data_len];
    for (uint8_t i = 1; i <= _SECURITY_ZONE_COUNT; i++)
    {
        snprintf_P(topic, sizeof(topic), ZONE_CONFIG_TOPIC_FORMAT, i);
        snprintf_P(data, sizeof(data), ZONE_CONFIG_DATA_FORMAT, i, i);

        this->PublishMessage(topic, data);
    }

    return 500 * TASK_MILLISECOND;
}

void SecuritySensor::ReadAndPublish()
{
    bool cur_val;
    bool changes = false;
    for (uint8_t i = 0; i < _SECURITY_ZONE_COUNT; i++)
    {
        cur_val = digitalRead(this->zone_pins[i]) == PinStatus::LOW;
        changes |= cur_val != this->zone_last_val[i];
        this->zone_last_val[i] = cur_val;
    }

    if (!changes)
    {
        return;
    }

    auto zone_len = strlen(ZONE_DATA_FORMAT) + (_SECURITY_ZONE_COUNT * 3) + 1;
    char payload[zone_len];
    sprintf(
        payload,
        ZONE_DATA_FORMAT,
        BOOL_STR(this->zone_last_val[0]),
        BOOL_STR(this->zone_last_val[1]),
        BOOL_STR(this->zone_last_val[2]),
        BOOL_STR(this->zone_last_val[3]),
        BOOL_STR(this->zone_last_val[4]),
        BOOL_STR(this->zone_last_val[5]));

    this->PublishMessage(SECURITY_STATE_TOPIC, payload);
}
