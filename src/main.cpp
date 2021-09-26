#define _TASK_OO_CALLBACKS

#include <TaskScheduler.h>
#include <WiFi.h>
#include "common.h"
#include "secrets.h"
#include "garageDoor.h"
#include "DHT.h"

#include "WifiConnectionService.h"
#include "MqttConnectionService.h"

#ifdef TEMPERATURE_SENSOR
DHT temp_sensor(4u, DHT22);
#endif

String baseTopic = String(SENSOR_MQTT_LOCATION) + "/" + SENSOR_UNIQUE_NAME;
Scheduler runner;
WiFiClient wificlient;
PubSubClient mqttClient(wificlient);

WifiConnectionService wifiService(&runner);
MqttConnectionService mqttService(&runner, &wifiService, &mqttClient);

#ifdef GARAGE_DOOR_SENSOR
GarageSensor garage_sensor(&runner, PIN_A1, PIN_A7);
#endif

void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * TASK_SECOND);
    }

    #ifdef GARAGE_DOOR_SENSOR
    garage_sensor.init();
    #endif

    #ifdef TEMPERATURE_SENSOR
    temp_sensor.begin();
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

#ifdef TEMPERATURE_SENSOR
void run_temp_sensor();

Task temperatureTask(5 * TASK_MINUTE, TASK_FOREVER, run_temp_sensor, &runner, true);

String temp_topic = baseTopic + "/temperature_F";
String humid_topic = baseTopic + "/humidity";

void run_temp_sensor()
{
    if (!mqttClient.connected())
    {
        delay(1 * TASK_SECOND);
        temperatureTask.forceNextIteration();
        return;
    }

    float temp = temp_sensor.readTemperature(true);
    float humidity = temp_sensor.readHumidity();

    DEBUG_TIME();
    DEBUG_SERIAL.print(temp);
    DEBUG_SERIAL.print(" °F, ");
    DEBUG_SERIAL.print(humidity);
    DEBUG_SERIAL.println(" %");

    if (isinf(temp) || isnan(temp) || isinf(humidity) || isnan(humidity))
    {
        return;
    }

    mqttClient.publish(temp_topic.c_str(), String(temp).c_str(), true);
    mqttClient.publish(humid_topic.c_str(), String(humidity).c_str(), true);
}

#endif