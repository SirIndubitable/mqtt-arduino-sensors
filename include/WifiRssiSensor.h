#ifndef __WIFI_RSSI_SENSOR_H__
#define __WIFI_RSSI_SENSOR_H__

#include "MqttSensor.h"

class WifiRssiSensor : public MqttSensor
{
    public:
        WifiRssiSensor(Scheduler* aScheduler, PubSubClient* mqttClient);
        void Init();
    protected:
        uint32_t OnMqttConnected();
        void ReadAndPublish();
};

#endif
