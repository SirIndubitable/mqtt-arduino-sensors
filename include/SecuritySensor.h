#ifndef __SECURITY_SENSOR_H__
#define __SECURITY_SENSOR_H__

#include <Arduino.h>
#include "MqttSensor.h"

#define _SECURITY_ZONE_COUNT 6u

class SecuritySensor : public MqttSensor
{
    public:
        SecuritySensor(
            Scheduler* aScheduler,
            PubSubClient* mqttClient,
            pin_size_t* zone_pin_arry);
        void Init();
    protected:
        uint32_t OnMqttConnected();
        void ReadAndPublish();
    private:
        pin_size_t zone_pins[_SECURITY_ZONE_COUNT];
        bool zone_last_val[_SECURITY_ZONE_COUNT];
};

#endif