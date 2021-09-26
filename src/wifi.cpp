#include <WiFi.h>
#include "wifiTask.h"
#include "common.h"
#include "secrets.h"

enum class WifiTaskState
{
    NotInitialized,
    Error,
    Connected,
    Connecting
};

WifiTaskState statusToState(uint8_t status)
{
    switch ((wl_status_t)status)
    {
        case wl_status_t::WL_NO_MODULE:
            return WifiTaskState::Error;

        case wl_status_t::WL_CONNECTED:
            return WifiTaskState::Connected;

        default:
            return WifiTaskState::Connecting;
    }
}

String statusToString(uint8_t status)
{
    switch ((wl_status_t)status)
    {
        case WL_CONNECTED:
            return "Wifi connected";
        case WL_NO_SHIELD:
            return "no wifi shield";
        case WL_IDLE_STATUS:
            return "wifi idle";
        case WL_CONNECT_FAILED:
            return "wifi connect failed";
        case WL_CONNECTION_LOST:
            return "wifi connection lost";
        case WL_DISCONNECTED:
            return "wifi disconnected";
        default:
            return "wifi other";
    }
}

WifiTask::WifiTask(Scheduler* aScheduler) : Task(TASK_IMMEDIATE, TASK_ONCE, aScheduler, false)
{
    this->state = WifiTaskState::NotInitialized;
}

void WifiTask::Init(String hostname)
{
    WiFi.setHostname("MyArduino");
    this->stateTransition(WiFi.status());
}

bool WifiTask::IsConnected()
{
    return WiFi.status() == wl_status_t::WL_CONNECTED;
}

bool WifiTask::Callback()
{
    switch (this->state)
    {
        case WifiTaskState::Connected:
            this->monitor();
            break;
        case WifiTaskState::Connecting:
            this->tryConnect();
            break;
        default:
            break;
    }

    return true;
}

bool WifiTask::stateTransition(uint8_t wifiStatus)
{
    auto newState = statusToState((wl_status_t)wifiStatus);
    if (this->state == newState)
    {
        return false;
    }

    this->state = newState;
    switch (this->state)
    {
        case WifiTaskState::Connected:
            this->setInterval(30 * TASK_SECOND);
            this->setIterations(TASK_FOREVER);
            this->enableIfNot();
            break;
        case WifiTaskState::Connecting:
            this->setInterval(10 * TASK_SECOND);
            this->setIterations(TASK_FOREVER);
            this->enableIfNot();
            break;
        default:
            this->disable();
            break;
    }

    return true;
}

void WifiTask::tryConnect()
{
    DEBUG_TIME();
    DEBUG_SERIAL.println("Connecting to Wifi");

    auto status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    if (this->stateTransition(status))
    {
        return;
    }

    DEBUG_TIME();
    DEBUG_SERIAL.print("ERROR - ");
    DEBUG_SERIAL.println(statusToString(status));
}

void WifiTask::monitor()
{
    auto status = WiFi.status();
    if (this->stateTransition(status))
    {
        DEBUG_TIME();
        DEBUG_SERIAL.println(statusToString(status));
    }
    #ifdef PUBLISH_WIFI_RSSI
    else
    {
        String rssi_topic = baseTopic + "/wifi/rssi";
        char rssi[12];
        itoa(WiFi.RSSI(), rssi, 10);
        DEBUG_TIME();
        DEBUG_SERIAL.print("Wifi RSSI: ");
        DEBUG_SERIAL.print(rssi);
        DEBUG_SERIAL.println(" dBm");
        mqttClient.publish(rssi_topic.c_str(), rssi);
    }
    #endif
}
