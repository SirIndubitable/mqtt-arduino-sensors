#include <Arduino.h>
#include "WifiRssiSensor.h"
#include "common.h"
#include "WiFi.h"

#ifndef SENSOR_UNIQUE_NAME
#define SENSOR_UNIQUE_NAME "example-name"
#endif

#define WIFI_STATE_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-wifi/state"

#define WIFI_CONFIG_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-wifi/config"
#define WIFI_CONFIG_DATA \
"{ " \
"\"name\": \"" SENSOR_UNIQUE_NAME " Wifi Strength\", " \
"\"dev_cla\": \"signal_strength\", " \
"\"stat_t\": \"" WIFI_STATE_TOPIC "\", " \
"\"unit_of_meas\": \"dBm\"" \
"}"

WifiRssiSensor::WifiRssiSensor(Scheduler* aScheduler, PubSubClient* mqttClient)
    : MqttSensor(aScheduler, mqttClient)
{
}

void WifiRssiSensor::Init()
{
}

uint32_t WifiRssiSensor::OnMqttConnected()
{
    this->PublishMessage(WIFI_CONFIG_TOPIC, WIFI_CONFIG_DATA);

    return 5 * TASK_MINUTE;
}

void WifiRssiSensor::ReadAndPublish()
{
    char rssi[12];
    itoa(WiFi.RSSI(), rssi, 10);

    this->PublishMessage(WIFI_STATE_TOPIC, rssi);
}