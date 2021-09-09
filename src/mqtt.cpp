#include "common.h"
#include "secrets.h"

void waitForWifi();
void tryConnectMqtt();
void monitorMqttConnection();

Task waitForWifiTask(1 * SECONDS,  TASK_FOREVER, waitForWifi,           &runner, true);
Task mqttConnectTask(5 * SECONDS,  TASK_FOREVER, tryConnectMqtt,        &runner, false);
Task mqttMonitorTask(30 * SECONDS, TASK_FOREVER, monitorMqttConnection, &runner, false);

bool mqtt_state_transition()
{
    // If we have no wifi, go to the waiting for wifi task
    if (WiFi.status() != wl_status_t::WL_CONNECTED)
    {
        mqttConnectTask.disable();
        mqttMonitorTask.disable();
        return !waitForWifiTask.enableIfNot();
    }

    // If we've connected to mqtt, just monitor the connection for downtime
    if (mqttClient.connected())
    {
        waitForWifiTask.disable();
        mqttConnectTask.disable();
        return !mqttMonitorTask.enableIfNot();
    }

    // If we have wifi, but aren't connected to mqtt, we should probably try to connect
    waitForWifiTask.disable();
    mqttMonitorTask.disable();
    return !mqttConnectTask.enableIfNot();
}

void waitForWifi()
{
    mqtt_state_transition();
}

void tryConnectMqtt()
{
    DEBUG_TIME();
    DEBUG_SERIAL.println("Connecting to MQTT");
    mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

    if (mqtt_state_transition())
    {
        return;
    }

    DEBUG_TIME();
    DEBUG_SERIAL.print("mqtt failed, rc=");
    DEBUG_SERIAL.println(mqttClient.state());
}

void monitorMqttConnection()
{
    if (mqtt_state_transition())
    {
        DEBUG_TIME();
        DEBUG_SERIAL.print("mqtt failed, rc=");
        DEBUG_SERIAL.println(mqttClient.state());
    }
}
