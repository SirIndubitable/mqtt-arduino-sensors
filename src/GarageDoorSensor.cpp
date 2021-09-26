#include <Arduino.h>
#include "common.h"
#include "GarageDoorSensor.h"

enum class GarageDoorSensorState
{
    Closed,
    Closing,
    Opened,
    Opening,
    Unknown,
    Initializing
};

String status_tostring(GarageDoorSensorState status)
{
    switch (status)
    {
        case GarageDoorSensorState::Closed:
            return "\"closed\"";
        case GarageDoorSensorState::Closing:
            return "\"closing\"";
        case GarageDoorSensorState::Opened:
            return "\"opened\"";
        case GarageDoorSensorState::Opening:
            return "\"opening\"";
        case GarageDoorSensorState::Initializing:
            return "\"initializing\"";
        case GarageDoorSensorState::Unknown:
        default:
            return "\"unknown\"";
    }
}

GarageDoorSensor::GarageDoorSensor(Scheduler* aScheduler, uint32_t open_limit_pin, uint32_t close_limit_pin)
    : Task(500 * TASK_MILLISECOND, TASK_FOREVER, aScheduler, true)
{
    this->topic = baseTopic + "/door/status";
    this->state = GarageDoorSensorState::Initializing;
    this->open_limit_pin = open_limit_pin;
    this->close_limit_pin = close_limit_pin;
}

void GarageDoorSensor::init()
{
    pinMode(this->open_limit_pin, PinMode::INPUT);
    pinMode(this->open_limit_pin, PinMode::INPUT);
}

bool GarageDoorSensor::Callback()
{
    // No need to publish a new value if nothing changed
    GarageDoorSensorState new_state = get_new_state();
    if (this->state == new_state)
    {
        return true;
    }

    String status_str = status_tostring(new_state);

    // Only change the actual state if it was succesffully published
    if (mqttClient.publish(this->topic.c_str(), status_str.c_str(), true))
    {
        DEBUG_TIME();
        DEBUG_SERIAL.println(status_str);
        this->state = new_state;
    }

    return true;
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
        if (this->state == GarageDoorSensorState::Opened
         || this->state == GarageDoorSensorState::Closing)
        {
            return GarageDoorSensorState::Closing;
        }
        if (this->state == GarageDoorSensorState::Closed
         || this->state == GarageDoorSensorState::Opening)
        {
            return GarageDoorSensorState::Opening;
        }
    }

    return GarageDoorSensorState::Unknown;
}
