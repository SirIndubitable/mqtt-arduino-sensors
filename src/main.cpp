#include <TaskScheduler.h>
#include "common.h"
#include "secrets.h"

String baseTopic = "home/floor0/garage";

#ifdef GARAGE_DOOR_SENSOR

#include "garageDoor.h"
GarageSensor sensor(baseTopic, PIN_A1, PIN_A2);

#endif

void toggle();
void runSensor();

Scheduler runner;

Task notifyTask(500 * TASK_MILLISECOND, TASK_FOREVER, runSensor, &runner, true);

WiFiSSLClient wificlient;
PubSubClient mqttClient(wificlient);

void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * TASK_SECOND);
    }

    sensor.init();

    WiFi.setHostname("MyArduino");
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    runner.startNow();
}

void loop()
{
    runner.execute();
    mqttClient.loop();
}

void runSensor()
{
    sensor.run();
}