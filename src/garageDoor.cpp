#include <Arduino.h>
#include "common.h"
#include "garageDoor.h"


String baseTopic = "home/floor0/garage/door/";
String statusTopic = baseTopic + "status";


String status_tostring(PinStatus status)
{
    switch (status)
    {
        case PinStatus::HIGH:
            return "\"on\"";
        case PinStatus::LOW:
            return "\"off\"";
        default:
            return "\"unknown\"";
    }
}

GarageSensor::GarageSensor()
{
    state = GargeSensorState::Initializing;
}

PinStatus lastStatus = PinStatus::CHANGE;
void GarageSensor::run()
{
    PinStatus curStatus = digitalRead(LED_BUILTIN);
    if (curStatus == lastStatus)
    {
        return;
    }
    lastStatus = curStatus;

    String status_str = status_tostring(lastStatus);
    DEBUG_TIME();
    DEBUG_SERIAL.println(status_str);
    mqttClient.publish(statusTopic.c_str(), status_str.c_str());
}
