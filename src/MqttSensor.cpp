#include "MqttSensor.h"
#include "common.h"

#define MQTT_WAITING_INTERVAL 500 * TASK_MILLISECOND

enum class MqttSensorState
{
    Waiting,
    Connected
};

MqttSensor::MqttSensor(Scheduler* aScheduler, PubSubClient* mqttClient)
    : Task(MQTT_WAITING_INTERVAL, TASK_FOREVER, aScheduler, true)
{
    this->mqttClient = mqttClient;
    this->mqttState = MqttSensorState::Waiting;
}

bool MqttSensor::Callback()
{
    auto newState = this->mqttClient->connected() ? MqttSensorState::Connected : MqttSensorState::Waiting;
    this->StateTransition(newState);

    if (this->mqttState == MqttSensorState::Connected)
    {
        this->ReadAndPublish();
    }

    return true;
}

void MqttSensor::StateTransition(MqttSensorState newState)
{
    if (this->mqttState == newState)
    {
        return;
    }

    this->mqttState = newState;
    switch(this->mqttState)
    {
        case MqttSensorState::Connected:
        {
            auto newInterval = this->OnMqttConnected();
            if (this->mqttState != MqttSensorState::Connected)
            {
                // If calling this->OnMqttConnected() transitioned states, don't keep
                // trying to set this state information
                break;
            }
            this->setInterval(newInterval);
            this->enableIfNot();
            break;
        }
        case MqttSensorState::Waiting:
        {
            this->setInterval(MQTT_WAITING_INTERVAL);
            this->enableIfNot();
            break;
        }
        default:
            this->disable();
            break;
    }
}

bool MqttSensor::PublishMessage(const char* topic, const char* payload)
{
    DEBUG_TIME();
    DEBUG_SERIAL.print(topic);
    DEBUG_SERIAL.print(": ");
    DEBUG_SERIAL.print(payload);
    DEBUG_SERIAL.println();

    auto success = this->mqttClient->publish(topic, payload, true);
    if (!success)
    {
        DEBUG_TIME();
        DEBUG_SERIAL.println("Publish failed");
        this->StateTransition(MqttSensorState::Waiting);
    }

    return success;
}