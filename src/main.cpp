// #include <Client.h>
// #include <SPI.h>
#include <TaskScheduler.h>
#include "common.h"
#include "secrets.h"

String baseTopic = "home/floor0/garage/door/";
String statusTopic = baseTopic + "status";

void toggle();
void notifyStatus();

Scheduler runner;
Task mqttTask();
Task toggleTask(5 * SECONDS, TASK_FOREVER, toggle, &runner, true);
Task notifyTask(5, TASK_FOREVER, notifyStatus, &runner, true);

WiFiSSLClient wificlient;
PubSubClient mqttClient(wificlient);

void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * SECONDS);
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

String status_tostring(PinStatus status)
{
    switch (status)
    {
        case PinStatus::HIGH:
            return "\"on\"";
        case PinStatus::LOW:
            return "\"off\"";
        default:
            return "\"unknown\"";
    }
}

PinStatus lastStatus = PinStatus::CHANGE;
void notifyStatus()
{
    PinStatus curStatus = digitalRead(LED_BUILTIN);
    if (curStatus == lastStatus)
    {
        return;
    }
    lastStatus = curStatus;

    String status_str = status_tostring(lastStatus);
    DEBUG_TIME();
    DEBUG_SERIAL.println(status_str);
    mqttClient.publish(statusTopic.c_str(), status_str.c_str());
}
