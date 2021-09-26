#include <WiFi.h>
#include "WifiConnectionService.h"
#include "common.h"
#include "secrets.h"

enum class WifiConnectionServiceState
{
    NotInitialized,
    Error,
    Connected,
    Connecting
};

WifiConnectionServiceState statusToState(uint8_t status)
{
    switch ((wl_status_t)status)
    {
        case wl_status_t::WL_NO_MODULE:
            return WifiConnectionServiceState::Error;

        case wl_status_t::WL_CONNECTED:
            return WifiConnectionServiceState::Connected;

        default:
            return WifiConnectionServiceState::Connecting;
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

WifiConnectionService::WifiConnectionService(Scheduler* aScheduler)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, aScheduler, false)
{
    this->state = WifiConnectionServiceState::NotInitialized;
}

void WifiConnectionService::Init(String hostname)
{
    WiFi.setHostname("MyArduino");
    this->stateTransition(WiFi.status());
}

bool WifiConnectionService::IsConnected()
{
    return WiFi.status() == wl_status_t::WL_CONNECTED;
}

bool WifiConnectionService::Callback()
{
    switch (this->state)
    {
        case WifiConnectionServiceState::Connected:
            this->monitor();
            break;
        case WifiConnectionServiceState::Connecting:
            this->tryConnect();
            break;
        default:
            break;
    }

    return true;
}

bool WifiConnectionService::stateTransition(uint8_t wifiStatus)
{
    auto newState = statusToState((wl_status_t)wifiStatus);
    if (this->state == newState)
    {
        return false;
    }

    this->state = newState;
    switch (this->state)
    {
        case WifiConnectionServiceState::Connected:
            this->setInterval(30 * TASK_SECOND);
            this->enableIfNot();
            break;
        case WifiConnectionServiceState::Connecting:
            this->setInterval(10 * TASK_SECOND);
            this->enableIfNot();
            break;
        default:
            this->disable();
            break;
    }

    return true;
}

void WifiConnectionService::tryConnect()
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

void WifiConnectionService::monitor()
{
    auto status = WiFi.status();
    if (this->stateTransition(status))
    {
        DEBUG_TIME();
        DEBUG_SERIAL.println(statusToString(status));
    }
}
