#include <Arduino.h>
#include "common.h"
#include "GarageDoorSensor.h"

#ifndef SENSOR_UNIQUE_NAME
#define SENSOR_UNIQUE_NAME "example-name"
#endif

#define GDOOR_STATE_TOPIC "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "-gdoor/state"
#define STATE_DATA_FORMAT "{ \"moving\": %s, \"garage_door\": %s }"

#define OPEN_CONFIG_TOPIC "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "-open/config"
#define OPEN_CONFIG_DATA \
"{ " \
"\"name\": \"" SENSOR_UNIQUE_NAME " Open\", " \
"\"dev_cla\": \"garage_door\", " \
"\"stat_t\": \"" GDOOR_STATE_TOPIC "\", " \
"\"val_tpl\": \"{{ value_json.garage_door }}\" " \
"}"

#define MOVING_CONFIG_TOPIC "homeassistant/binary_sensor/" SENSOR_UNIQUE_NAME "-moving/config"
#define MOVING_CONFIG_DATA \
"{ " \
"\"name\": \"" SENSOR_UNIQUE_NAME " Moving\", " \
"\"dev_cla\": \"moving\", " \
"\"stat_t\": \"" GDOOR_STATE_TOPIC "\", " \
"\"val_tpl\": \"{{ value_json.moving }}\" " \
"}"

enum class GarageDoorSensorState
{
    Opened,
    Moving,
    Closed,
    Unknown
};

GarageDoorSensor::GarageDoorSensor(Scheduler* aScheduler, PubSubClient* mqttClient, uint32_t open_limit_pin, uint32_t close_limit_pin)
    : MqttSensor(aScheduler, mqttClient)
{
    this->door_state = GarageDoorSensorState::Unknown;
    this->open_limit_pin = open_limit_pin;
    this->close_limit_pin = close_limit_pin;
}

void GarageDoorSensor::Init()
{
    pinMode(this->open_limit_pin, PinMode::INPUT);
    pinMode(this->open_limit_pin, PinMode::INPUT);
}

uint32_t GarageDoorSensor::OnMqttConnected()
{
    this->door_state = GarageDoorSensorState::Unknown;
    this->PublishMessage(OPEN_CONFIG_TOPIC, OPEN_CONFIG_DATA);
    this->PublishMessage(MOVING_CONFIG_TOPIC, MOVING_CONFIG_DATA);

    return 500 * TASK_MILLISECOND;
}

void GarageDoorSensor::ReadAndPublish()
{
    // No need to publish a new value if nothing changed
    auto new_state = get_new_state();
    if (this->door_state == new_state)
    {
        return;
    }

    this->door_state = new_state;

    // Because we consider it open when it's moving and when it's open, check not closed
    const char* open = this->door_state != GarageDoorSensorState::Closed ? "true" : "false";
    const char* moving = this->door_state == GarageDoorSensorState::Moving ? "true" : "false";

    char payload[42];
    snprintf_P(payload, sizeof(payload), STATE_DATA_FORMAT, moving, open);

    this->PublishMessage(GDOOR_STATE_TOPIC, payload);
}

GarageDoorSensorState GarageDoorSensor::get_new_state()
{
    PinStatus close_limit = digitalRead(this->close_limit_pin);
    PinStatus open_limit = digitalRead(this->open_limit_pin);

    // These switches are normally open reed switches
    // When the they are active, it pulls the voltage to Low
    bool close_switch_active = close_limit == PinStatus::LOW;
    bool open_switch_active = open_limit == PinStatus::LOW;

    if (close_switch_active && !open_switch_active)
    {
        return GarageDoorSensorState::Closed;
    }
    if (open_switch_active && !close_switch_active)
    {
        return GarageDoorSensorState::Opened;
    }
    if (!close_switch_active && !open_switch_active)
    {
        return GarageDoorSensorState::Moving;
    }

    return GarageDoorSensorState::Unknown;
}
