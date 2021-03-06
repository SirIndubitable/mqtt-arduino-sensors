#define _TASK_OO_CALLBACKS

#include <TaskScheduler.h>
#include <WiFi.h>
#include "common.h"
#include "secrets.h"

#include "WifiConnectionService.h"
#include "MqttConnectionService.h"

Scheduler runner;
WiFiClient wificlient;
PubSubClient mqttClient(wificlient);

WifiConnectionService wifiService(&runner);
MqttConnectionService mqttService(&runner, &wifiService, &mqttClient);

#if defined(SECURITY_SENSOR)

#include "SecuritySensor.h"
pin_size_t security_pins[] = { 2u, 3u, 4u, 5u, 6u, 7u };
MqttSensor* sensors[]
{
    new SecuritySensor(&runner, &mqttClient, security_pins),
};

#elif defined(GARAGE_DOOR_SENSOR)

#include "GarageDoorSensor.h"
#include "ClimateSensor.h"
#include "WifiRssiSensor.h"

MqttSensor* sensors[]
{
    new GarageDoorSensor(&runner, &mqttClient, PIN_A1, PIN_A7),
    new ClimateSensor(&runner, &mqttClient, 4u, DHT22),
    new WifiRssiSensor(&runner, &mqttClient),
};

#endif

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
