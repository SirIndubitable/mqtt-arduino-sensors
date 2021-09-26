#ifndef __MQTTSENSOR_H__
#define __MQTTSENSOR_H__

#define _TASK_OO_CALLBACKS

#include <TaskSchedulerDeclarations.h>
#include <PubSubClient.h>

enum class MqttSensorState;

class MqttSensor : public Task
{
    public:
        MqttSensor(Scheduler* aScheduler, PubSubClient* mqttClient);
        bool Callback();
    protected:
        bool PublishMessage(const char* topic, const char* payload);
        virtual uint32_t OnMqttConnected() = 0;
        virtual void ReadAndPublish() = 0;
    private:
        PubSubClient* mqttClient;
        MqttSensorState mqttState;
        void StateTransition(MqttSensorState newState);
};

#endif