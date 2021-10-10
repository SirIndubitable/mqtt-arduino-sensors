#include "common.h"
#include "MqttConnectionService.h"

enum class MqttConnectionServiceState
{
    NotInitialized,
    Error,
    Connected,
    Connecting,
    WaitingForWifi
};

void _print_mqtt_state(int32_t state)
{
    switch(state)
    {
        case 0:
            DEBUG_SERIAL.print("Connection Accepted");
            break;
        case -1:
            DEBUG_SERIAL.print("Protocall verion denied");
            break;
        case -2:
            DEBUG_SERIAL.print("Id rejected");
            break;
        case -3:
            DEBUG_SERIAL.print("Service unavailable");
            break;
        case -4:
            DEBUG_SERIAL.print("Invalid credentials");
            break;
        case -5:
            DEBUG_SERIAL.print("Client not authorized");
            break;
        default:
            DEBUG_SERIAL.print("Unkown error: ");
            DEBUG_SERIAL.print(state);
            break;
    }
}

MqttConnectionService::MqttConnectionService(Scheduler* aScheduler, WifiConnectionService* wifiService, PubSubClient* mqttClient)
: Task(TASK_IMMEDIATE, TASK_FOREVER, aScheduler, false)
{
    this->state = MqttConnectionServiceState::NotInitialized;
    this->wifiService = wifiService;
    this->mqttClient = mqttClient;
}

void MqttConnectionService::Init(const char* hostname, uint16_t port, const char* client_id, const char* username, const char* password)
{
    this->mqttClient->setServer(hostname, port);
    this->client_id = client_id;
    this->username = username;
    this->password = password;
    this->stateTransition();
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
            DEBUG_TIME();
            DEBUG_SERIAL.println("MQTT Service - Connected");
            this->setInterval(30 * TASK_SECOND);
            this->enableIfNot();
            break;
        case MqttConnectionServiceState::Connecting:
            DEBUG_TIME();
            DEBUG_SERIAL.println("MQTT Service - Connecting");
            this->setInterval(5 * TASK_SECOND);
            this->enableIfNot();
            break;
        case MqttConnectionServiceState::WaitingForWifi:
            DEBUG_TIME();
            DEBUG_SERIAL.println("MQTT Service - Waiting for Wifi");
            this->setInterval(1 * TASK_SECOND);
            this->enableIfNot();
            break;
        default:
            DEBUG_SERIAL.println("MQTT Service - Unknown error (Disabled)");
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
    DEBUG_SERIAL.print("Connecting to MQTT - ");
    DEBUG_SERIAL.println(this->client_id);
    this->mqttClient->connect(this->client_id, this->username, this->password);

    if (this->stateTransition())
    {
        return;
    }

    _print_mqtt_state(this->mqttClient->state());
    DEBUG_SERIAL.println();
}

void MqttConnectionService::monitor()
{
    auto status = this->mqttClient->state();
    if (status < 0)
    {
        _print_mqtt_state(status);
        DEBUG_SERIAL.println();
    }

    this->stateTransition();
}