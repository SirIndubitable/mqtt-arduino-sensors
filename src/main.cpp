#define _TASK_OO_CALLBACKS

#include <TaskScheduler.h>
#include <WiFi.h>
#include "common.h"
#include "secrets.h"

#include "WifiConnectionService.h"
#include "MqttConnectionService.h"

#ifdef GARAGE_DOOR_SENSOR
#include "GarageDoorSensor.h"
#endif

#ifdef TEMPERATURE_SENSOR
#include "ClimateSensor.h"
#endif

#ifdef PUBLISH_WIFI_RSSI
#include "WifiRssiSensor.h"
#endif

#ifdef SECURITY_SENSOR
#include "SecuritySensor.h"
#endif

Scheduler runner;
WiFiClient wificlient;
PubSubClient mqttClient(wificlient);

WifiConnectionService wifiService(&runner);
MqttConnectionService mqttService(&runner, &wifiService, &mqttClient);

MqttSensor* sensors[]
{
    #ifdef GARAGE_DOOR_SENSOR
    new GarageDoorSensor(&runner, &mqttClient, PIN_A1, PIN_A7),
    #endif

    #ifdef TEMPERATURE_SENSOR
    new ClimateSensor(&runner, &mqttClient, 4u, DHT22),
    #endif

    #ifdef PUBLISH_WIFI_RSSI
    new WifiRssiSensor(&runner, &mqttClient),
    #endif

    #ifdef SECURITY_SENSOR
    new SecuritySensor(&runner, &mqttClient, new[] { PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5, PIN_A6 }),
    #endif
};


void setup()
{
    Serial.begin(9600);

    #if DEBUG
    while (!Serial)
    {
        delay(1 * TASK_SECOND);
    }
    #endif

    uint8_t num_sensors = sizeof(sensors)/sizeof(sensors[0]);
    for (uint8_t i=0; i<num_sensors; i++)
    {
        sensors[i]->Init();
    }

    wifiService.Init(SENSOR_UNIQUE_NAME);
    mqttService.Init(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

    runner.startNow();
}

void loop()
{
    runner.execute();
    mqttClient.loop();
}
