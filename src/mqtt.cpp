#include "common.h"
#include "secrets.h"
#include "mqttTask.h"

enum class MqttTaskState
{
    NotInitialized,
    Error,
    Connected,
    Connecting,
    WaitingForWifi
};

MqttTask::MqttTask(Scheduler* aScheduler, WifiTask* wifiTask, PubSubClient* mqttClient)
: Task(TASK_IMMEDIATE, TASK_ONCE, aScheduler, false)
{
    this->state = MqttTaskState::NotInitialized;
    this->wifiTask = wifiTask;
    this->mqttClient = mqttClient;
}

void MqttTask::Init(const char* hostname, uint8_t port, const char* client_id, const char* username, const char* password)
{
    this->mqttClient->setServer(hostname, port);
    this->client_id = client_id;
    this->username = username;
    this->password = password;
}

bool MqttTask::Callback()
{
    switch (this->state)
    {
        case MqttTaskState::Connected:
            this->monitor();
            break;
        case MqttTaskState::Connecting:
            this->tryConnect();
            break;
        case MqttTaskState::WaitingForWifi:
            this->waitForWifi();
            break;
        default:
            break;
    }

    return true;
}

MqttTaskState MqttTask::getNewState()
{
    if (!this->wifiTask->IsConnected())
    {
        return MqttTaskState::WaitingForWifi;
    }
    if (this->mqttClient->connected())
    {
        return MqttTaskState::Connected;
    }

    return MqttTaskState::Connecting;
}

bool MqttTask::stateTransition()
{
    auto newState = this->getNewState();
    if (this->state == newState)
    {
        return false;
    }

    this->state = newState;
    switch (this->state)
    {
        case MqttTaskState::Connected:
            this->setInterval(30 * TASK_SECOND);
            this->setIterations(TASK_FOREVER);
            this->enableIfNot();
            break;
        case MqttTaskState::Connecting:
            this->setInterval(5 * TASK_SECOND);
            this->setIterations(TASK_FOREVER);
            this->enableIfNot();
            break;
        case MqttTaskState::WaitingForWifi:
            this->setInterval(1 * TASK_SECOND);
            this->setIterations(TASK_FOREVER);
            this->enableIfNot();
            break;
        default:
            this->disable();
            break;
    }

    return true;
}

void MqttTask::waitForWifi()
{
    this->stateTransition();
}

void MqttTask::tryConnect()
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

void MqttTask::monitor()
{
    if (this->stateTransition())
    {
        DEBUG_TIME();
        DEBUG_SERIAL.print("mqtt failed, rc=");
        DEBUG_SERIAL.println(this->mqttClient->state());
    }
}