#ifndef __MQTTTASK_H__
#define __MQTTTASK_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <PubSubClient.h>
#include "wifiTask.h"

enum class MqttTaskState;

class MqttTask : public Task
{
    public:
        MqttTask(Scheduler* aScheduler, WifiTask* wifiTask, PubSubClient* mqttClient);
        void Init(const char* hostname, uint8_t port, const char* client_id, const char* username, const char* password);
        bool Callback();
    private:
        PubSubClient* mqttClient;
        WifiTask* wifiTask;
        MqttTaskState state;
        const char* client_id;
        const char* username;
        const char* password;
        MqttTaskState getNewState();
        bool stateTransition();
        void tryConnect();
        void monitor();
        void waitForWifi();
};


#endif
