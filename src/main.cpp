#include <TaskScheduler.h>
#include "common.h"
#include "secrets.h"

#ifdef GARAGE_DOOR_SENSOR

#include "garageDoor.h"
GarageSensor sensor;

#endif

void toggle();
void runSensor();

Scheduler runner;

Task toggleTask(5 * TASK_SECOND,      TASK_FOREVER, toggle, &runner, true);
Task notifyTask(5 * TASK_MILLISECOND, TASK_FOREVER, runSensor, &runner, true);\

WiFiSSLClient wificlient;
PubSubClient mqttClient(wificlient);

void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * TASK_SECOND);
    }

    WiFi.setHostname("MyArduino");
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    pinMode(LED_BUILTIN, OUTPUT);

    runner.startNow();
}

void loop()
{
    runner.execute();
    mqttClient.loop();
}

void toggle()
{
    DEBUG_TIME();
    DEBUG_SERIAL.println("Toggle");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void runSensor()
{
    sensor.run();
}