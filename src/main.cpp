#define _TASK_OO_CALLBACKS

#include <TaskScheduler.h>
#include <WiFi.h>
#include "common.h"
#include "secrets.h"
#include "DHT.h"

#include "WifiConnectionService.h"
#include "MqttConnectionService.h"

String baseTopic = String(SENSOR_MQTT_LOCATION) + "/" + SENSOR_UNIQUE_NAME;
Scheduler runner;
WiFiClient wificlient;
PubSubClient mqttClient(wificlient);

WifiConnectionService wifiService(&runner);
MqttConnectionService mqttService(&runner, &wifiService, &mqttClient);

#ifdef GARAGE_DOOR_SENSOR
#include "GarageDoorSensor.h"

GarageDoorSensor garage_sensor(&runner, &mqttClient, PIN_A1, PIN_A7);
#endif

#ifdef TEMPERATURE_SENSOR
#include "ClimateSensor.h"

ClimateSensor climate_sensor(&runner, &mqttClient, 4u, DHT22);
#endif

void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * TASK_SECOND);
    }

    #ifdef GARAGE_DOOR_SENSOR
    garage_sensor.Init();
    #endif

    #ifdef TEMPERATURE_SENSOR
    climate_sensor.Init();
    #endif

    wifiService.Init("MyArduino");
    mqttService.Init(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

    runner.startNow();
}

void loop()
{
    runner.execute();
    mqttClient.loop();
}
