#include "ClimateSensor.h"
#include "common.h"

ClimateSensor::ClimateSensor(Scheduler* aScheduler, uint8_t pin, uint8_t dhtType)
    : Task(5 * TASK_MINUTE, TASK_FOREVER, aScheduler, true)
    , tempSensor(pin, dhtType)
{
}

void ClimateSensor::Init()
{
    this->tempSensor.begin();
}

String temp_topic = String("homeassistant/sensor/") + SENSOR_UNIQUE_NAME + "/temperature_F";
String humid_topic = String("homeassistant/sensor/") + SENSOR_UNIQUE_NAME + "/humidity";
bool ClimateSensor::Callback()
{
    if (!mqttClient.connected())
    {
        delay(1 * TASK_SECOND);
        this->forceNextIteration();
        return true;
    }

    float temp = this->tempSensor.readTemperature(true);
    float humidity = this->tempSensor.readHumidity();

    DEBUG_TIME();
    DEBUG_SERIAL.print(temp);
    DEBUG_SERIAL.print(" °F, ");
    DEBUG_SERIAL.print(humidity);
    DEBUG_SERIAL.println(" %");

    if (isinf(temp) || isnan(temp) || isinf(humidity) || isnan(humidity))
    {
        return true;
    }

    mqttClient.publish(temp_topic.c_str(), String(temp).c_str(), true);
    mqttClient.publish(humid_topic.c_str(), String(humidity).c_str(), true);
    return true;
}