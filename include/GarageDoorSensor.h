#ifndef __GARAGE_DOOR_H__
#define __GARAGE_DOOR_H__

#include "MqttSensor.h"

enum class GarageDoorSensorState;

class GarageDoorSensor : public MqttSensor
{
    public:
        GarageDoorSensor(Scheduler* aScheduler, PubSubClient* mqttClient, uint32_t open_limit_pin, uint32_t close_limit_pin);
        void Init();
    protected:
        uint32_t OnMqttConnected();
        void ReadAndPublish();
    private:
        GarageDoorSensorState door_state;
        uint32_t open_limit_pin;
        uint32_t close_limit_pin;
        GarageDoorSensorState get_new_state();
};

#endif
