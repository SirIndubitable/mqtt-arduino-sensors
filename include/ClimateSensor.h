#ifndef __CLIMATESENSOR_H__
#define __CLIMATESENSOR_H__

#include <Arduino.h>
#include "MqttSensor.h"
#include "DHT.h"

class ClimateSensor : public MqttSensor
{
    public:
        ClimateSensor(Scheduler* aScheduler, PubSubClient* mqttClient, uint8_t pin, uint8_t dhtType);
        void Init();
    protected:
        uint32_t OnMqttConnected();
        void ReadAndPublish();
    private:
        DHT tempSensor;
};

#endif