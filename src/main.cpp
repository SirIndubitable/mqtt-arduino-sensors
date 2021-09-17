#include <TaskScheduler.h>
#include "common.h"
#include "secrets.h"
#include "garageDoor.h"

String baseTopic = "home/floor0/garage";

Scheduler runner;
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

#ifdef GARAGE_DOOR_SENSOR

GarageSensor sensor(baseTopic, PIN_A1, PIN_A2);

void run_garage_door()
{
    sensor.run();
}

Task notifyTask(500 * TASK_MILLISECOND, TASK_FOREVER, run_garage_door, &runner, true);

#endif