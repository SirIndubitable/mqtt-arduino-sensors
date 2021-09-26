#include "common.h"
#include "secrets.h"
#include "MqttConnectionService.h"

enum class MqttConnectionServiceState
{
    NotInitialized,
    Error,
    Connected,
    Connecting,
    WaitingForWifi
};

MqttConnectionService::MqttConnectionService(Scheduler* aScheduler, WifiConnectionService* wifiService, PubSubClient* mqttClient)
: Task(TASK_IMMEDIATE, TASK_FOREVER, aScheduler, false)
{
    this->state = MqttConnectionServiceState::NotInitialized;
    this->wifiService = wifiService;
    this->mqttClient = mqttClient;
}

void MqttConnectionService::Init(const char* hostname, uint8_t port, const char* client_id, const char* username, const char* password)
{
    this->mqttClient->setServer(hostname, port);
    this->client_id = client_id;
    this->username = username;
    this->password = password;
}

bool MqttConnectionService::Callback()
{
    switch (this->state)
    {
        case MqttConnectionServiceState::Connected:
            this->monitor();
            break;
        case MqttConnectionServiceState::Connecting:
            this->tryConnect();
            break;
        case MqttConnectionServiceState::WaitingForWifi:
            this->waitForWifi();
            break;
        default:
            break;
    }

    return true;
}

MqttConnectionServiceState MqttConnectionService::getNewState()
{
    if (!this->wifiService->IsConnected())
    {
        return MqttConnectionServiceState::WaitingForWifi;
    }
    if (this->mqttClient->connected())
    {
        return MqttConnectionServiceState::Connected;
    }

    return MqttConnectionServiceState::Connecting;
}

bool MqttConnectionService::stateTransition()
{
    auto newState = this->getNewState();
    if (this->state == newState)
    {
        return false;
    }

    this->state = newState;
    switch (this->state)
    {
        case MqttConnectionServiceState::Connected:
            this->setInterval(30 * TASK_SECOND);
            this->enableIfNot();
            break;
        case MqttConnectionServiceState::Connecting:
            this->setInterval(5 * TASK_SECOND);
            this->enableIfNot();
            break;
        case MqttConnectionServiceState::WaitingForWifi:
            this->setInterval(1 * TASK_SECOND);
            this->enableIfNot();
            break;
        default:
            this->disable();
            break;
    }

    return true;
}

void MqttConnectionService::waitForWifi()
{
    this->stateTransition();
}

void MqttConnectionService::tryConnect()
{
    DEBUG_TIME();
    DEBUG_SERIAL.println("Connecting to MQTT");
    DEBUG_SERIAL.print("   ");
    DEBUG_SERIAL.println(this->client_id);
    this->mqttClient->connect(this->client_id, this->username, this->password);

    if (this->stateTransition())
    {
        return;
    }

    DEBUG_TIME();
    DEBUG_SERIAL.print("mqtt failed, rc=");
    DEBUG_SERIAL.println(this->mqttClient->state());
}

void MqttConnectionService::monitor()
{
    if (this->stateTransition())
    {
        DEBUG_TIME();
        DEBUG_SERIAL.print("mqtt failed, rc=");
        DEBUG_SERIAL.println(this->mqttClient->state());
    }
}