#include <Arduino.h>
#include "common.h"
#include "SecuritySensor.h"

#ifndef SENSOR_UNIQUE_NAME
#define SENSOR_UNIQUE_NAME "example-name"
#endif

#define SECURITY_STATE_TOPIC "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "/state"
#define ZONE_1_STATE_DATA_FORMAT "\"z1\": \"%s\""
#define ZONE_N_STATE_DATA_FORMAT ", \"z%u\": \"%s\""

#define ZONE_CONFIG_TOPIC_FORMAT "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "-z%u/config"
#define ZONE_CONFIG_DATA_FORMAT \
"{ " \
"\"name\": \"Zone %1$u\", " \
"\"dev_cla\": \"opening\", " \
"\"stat_t\": \"" SECURITY_STATE_TOPIC "\", " \
"\"val_tpl\": \"{{ value_json.z%1$u }}\" " \
"}"

SecuritySensor::SecuritySensor(Scheduler* aScheduler, PubSubClient* mqttClient, pin_size_t* zone_pin_arry)
    : MqttSensor(aScheduler, mqttClient)
{
    for (int i = 0; i < _SECURITY_ZONE_COUNT; i++)
    {
        this->zone_pins[i] = zone_pin_arry[i];
    }
}

void SecuritySensor::Init()
{
    for (int i = 0; i < _SECURITY_ZONE_COUNT; i++)
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
    for (int i = 1; i <= _SECURITY_ZONE_COUNT; i++)
    {
        snprintf_P(topic, sizeof(topic), SECURITY_STATE_TOPIC, i);
        snprintf_P(data, sizeof(data), SECURITY_STATE_TOPIC, i);

        this->PublishMessage(topic, data);
    }

    return 500 * TASK_MILLISECOND;
}

void SecuritySensor::ReadAndPublish()
{
    auto zone1_len = strlen(ZONE_1_STATE_DATA_FORMAT) + 4;
    auto zoneN_len = strlen(ZONE_N_STATE_DATA_FORMAT) + 4;

    char payload[zone1_len + (zoneN_len * (_SECURITY_ZONE_COUNT - 1)) + 4];
    char* format_pointer = payload;

    strcpy(format_pointer, "{ ");

    for (int i = 1; i <= _SECURITY_ZONE_COUNT; i++)
    {
        format_pointer = &payload[strlen(payload)];

        auto open = this->zone_last_val[i-1] ? "true" : "false";
        if (i == 1)
        {
            sprintf(format_pointer, ZONE_1_STATE_DATA_FORMAT, open);
        }
        else
        {
            sprintf(format_pointer, ZONE_N_STATE_DATA_FORMAT, i, open);
        }
    }

    this->PublishMessage(SECURITY_STATE_TOPIC, payload);
}
