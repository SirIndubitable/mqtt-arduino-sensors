#ifndef __MQTTCONNECTIONSERVICE_H__
#define __MQTTCONNECTIONSERVICE_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <PubSubClient.h>
#include "WifiConnectionService.h"

enum class MqttConnectionServiceState;

class MqttConnectionService : public Task
{
    public:
        MqttConnectionService(Scheduler* aScheduler, WifiConnectionService* wifiService, PubSubClient* mqttClient);
        void Init(const char* hostname, uint8_t port, const char* client_id, const char* username, const char* password);
        bool Callback();
    private:
        PubSubClient* mqttClient;
        WifiConnectionService* wifiService;
        MqttConnectionServiceState state;
        const char* client_id;
        const char* username;
        const char* password;
        MqttConnectionServiceState getNewState();
        bool stateTransition();
        void tryConnect();
        void monitor();
        void waitForWifi();
};


#endif
