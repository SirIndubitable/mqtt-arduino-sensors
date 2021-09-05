#include <Arduino.h>
#include <Client.h>
#include <SPI.h>
#include <WiFi101.h>
#include <CooperativeMultitasking.h>
#include <MQTTClient.h>
#include "secrets.h"

#define SECONDS 1000

char topicname[] = "arduino-test";

CooperativeMultitasking tasks;
Continuation beginWiFiIfNeeded;
Continuation connectMQTTClientIfNeeded;
Continuation on;
Continuation off;

WiFiClient wificlient;
MQTTClient mqttclient(&tasks, &wificlient, MQTT_HOST, 1883, MQTT_CLIENT, MQTT_USERNAME, MQTT_PASSWORD);
MQTTTopic topic(&mqttclient, topicname);

void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * SECONDS);
    }

    Serial.println("begin wifi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    tasks.after(10 * SECONDS, beginWiFiIfNeeded);

    Serial.println("connect mqtt client");
    mqttclient.connect();
    tasks.after(15 * SECONDS, connectMQTTClientIfNeeded);

    pinMode(LED_BUILTIN, OUTPUT);
    tasks.now(on);
}

void loop()
{
    tasks.run();
}

void on()
{
    topic.publish("\"on\"");
    digitalWrite(LED_BUILTIN, HIGH);
    tasks.after(5 * SECONDS, off);
}

void off()
{
    topic.publish("\"off\"");
    digitalWrite(LED_BUILTIN, LOW);
    tasks.after(5 * SECONDS, on);
}

void beginWiFiIfNeeded()
{
    switch (WiFi.status())
    {
        case WL_IDLE_STATUS:
        case WL_CONNECTED:
            tasks.after(30 * SECONDS, beginWiFiIfNeeded);
            return;
        case WL_NO_SHIELD:
            Serial.println("no wifi shield");
            return;
        case WL_CONNECT_FAILED:
            Serial.println("wifi connect failed");
            break;
        case WL_CONNECTION_LOST:
            Serial.println("wifi connection lost");
            break;
        case WL_DISCONNECTED:
            Serial.println("wifi disconnected");
            break;
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    tasks.after(10 * SECONDS, beginWiFiIfNeeded);
}

void connectMQTTClientIfNeeded()
{
    if (!mqttclient.connected())
    {
        Serial.println("mqtt client not connected");
        mqttclient.connect();
    }

    tasks.after(30 * SECONDS, connectMQTTClientIfNeeded);
}
