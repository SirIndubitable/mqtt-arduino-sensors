#include <Arduino.h>
#include <Client.h>
#include <SPI.h>
#include <CooperativeMultitasking.h>
#include <PubSubClient.h>
#include "secrets.h"
#include <WiFi.h>

#define SECONDS 1000

char topicname[] = "testtopic/2";

CooperativeMultitasking tasks;
Continuation beginWiFiIfNeeded;
Continuation connectMQTTClientIfNeeded;
Continuation toggle;
Continuation notifyStatus;

WiFiSSLClient wificlient;
PubSubClient mqttclient(wificlient);


void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i=0; i<length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}


void setup()
{
    Serial.begin(9600);

    while (!Serial)
    {
        delay(1 * SECONDS);
    }

    WiFi.setHostname("MyArduino");
    beginWiFiIfNeeded();

    mqttclient.setServer(MQTT_HOST, MQTT_PORT);
    mqttclient.setCallback(callback);
    connectMQTTClientIfNeeded();

    pinMode(LED_BUILTIN, OUTPUT);
    tasks.now(toggle);
    tasks.now(notifyStatus);
}

void loop()
{
    tasks.run();
    mqttclient.loop();
}

void toggle()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    tasks.after(5 * SECONDS, toggle);
}

void notifyStatus()
{
    switch (digitalRead(LED_BUILTIN))
    {
        case PinStatus::HIGH:
            Serial.println("on");
            mqttclient.publish(topicname, "\"on\"");
            break;
        case PinStatus::LOW:
            Serial.println("off");
            mqttclient.publish(topicname, "\"off\"");
            break;
        default:
            break;
    }

    tasks.after(5 * SECONDS, notifyStatus);
}

void beginWiFiIfNeeded()
{
    switch (WiFi.status())
    {
        case WL_CONNECTED:
            Serial.println("Wifi connected");
            tasks.after(30 * SECONDS, beginWiFiIfNeeded);
            return;
        case WL_NO_SHIELD:
            Serial.println("no wifi shield");
            return;
        case WL_IDLE_STATUS:
            Serial.println("wifi idle");
            break;
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
    if (WiFi.status() != WL_CONNECTED)
    {
        // If we have no wifi, we can't connect to MQTT, so wait a second
        tasks.after(1 * SECONDS, connectMQTTClientIfNeeded);
        return;
    }

    if (mqttclient.connected() || mqttclient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
        tasks.after(30 * SECONDS, connectMQTTClientIfNeeded);
        return;
    }

    Serial.print("mqtt failed, rc=");
    Serial.print(mqttclient.state());
    Serial.println(" try again in 5 seconds");
    tasks.after(5 * SECONDS, connectMQTTClientIfNeeded);
}
