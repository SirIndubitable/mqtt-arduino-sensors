#ifdef GARAGE_DOOR_SENSOR
#include <Arduino.h>
#include "common.h"
#include "garageDoor.h"

enum class GargeSensorState
{
    Closed,
    Closing,
    Opened,
    Opening,
    Unknown,
    Initializing
};

String status_tostring(GargeSensorState status)
{
    switch (status)
    {
        case GargeSensorState::Closed:
            return "\"closed\"";
        case GargeSensorState::Closing:
            return "\"closing\"";
        case GargeSensorState::Opened:
            return "\"opened\"";
        case GargeSensorState::Opening:
            return "\"opening\"";
        case GargeSensorState::Initializing:
            return "\"initializing\"";
        case GargeSensorState::Unknown:
        default:
            return "\"unknown\"";
    }
}

GarageSensor::GarageSensor(Scheduler* aScheduler, uint32_t open_limit_pin, uint32_t close_limit_pin)
    : Task(500 * TASK_MILLISECOND, TASK_FOREVER, aScheduler, true)
{
    this->topic = baseTopic + "/door/status";
    this->state = GargeSensorState::Initializing;
    this->open_limit_pin = open_limit_pin;
    this->close_limit_pin = close_limit_pin;
}

void GarageSensor::init()
{
    pinMode(this->open_limit_pin, PinMode::INPUT);
    pinMode(this->open_limit_pin, PinMode::INPUT);
}

bool GarageSensor::Callback()
{
    // No need to publish a new value if nothing changed
    GargeSensorState new_state = get_new_state();
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


GargeSensorState GarageSensor::get_new_state()
{
    PinStatus close_limit = digitalRead(this->close_limit_pin);
    PinStatus open_limit = digitalRead(this->open_limit_pin);

    // These switches are normally open reed switches
    // When the they are active, it pulls the voltage to Low
    bool close_switch_active = close_limit == PinStatus::LOW;
    bool open_switch_active = open_limit == PinStatus::LOW;

    if (close_switch_active && !open_switch_active)
    {
        return GargeSensorState::Closed;
    }
    if (open_switch_active && !close_switch_active)
    {
        return GargeSensorState::Opened;
    }
    if (!close_switch_active && !open_switch_active)
    {
        if (this->state == GargeSensorState::Opened
         || this->state == GargeSensorState::Closing)
        {
            return GargeSensorState::Closing;
        }
        if (this->state == GargeSensorState::Closed
         || this->state == GargeSensorState::Opening)
        {
            return GargeSensorState::Opening;
        }
    }

    return GargeSensorState::Unknown;
}
#endif