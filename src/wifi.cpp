#include "common.h"
#include "secrets.h"

void tryConnectWifi();
void monitorWifi();

Task wifiConnectTask(10 * SECONDS, TASK_FOREVER, tryConnectWifi, &runner, true);
Task wifiMonitorTask(30 * SECONDS, TASK_FOREVER, monitorWifi,    &runner, false);

String status_tostring(wl_status_t status)
{
    switch (status)
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

void tryConnectWifi()
{
    wl_status_t status = (wl_status_t)WiFi.status();
    DEBUG_TIME();
    DEBUG_SERIAL.print("[tryConnectWifi] ");
    DEBUG_SERIAL.println(status_tostring(status));

    if (status == wl_status_t::WL_NO_MODULE)
    {
        wifiConnectTask.disable();
        wifiMonitorTask.disable();
        return;
    }
    if (status != wl_status_t::WL_CONNECTED)
    {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    else
    {
        wifiConnectTask.disable();
        wifiMonitorTask.enable();
    }
}

void monitorWifi()
{
    wl_status_t status = (wl_status_t)WiFi.status();
    DEBUG_TIME();
    DEBUG_SERIAL.print("[monitorWifi] ");
    DEBUG_SERIAL.println(status_tostring(status));

    if (status != wl_status_t::WL_CONNECTED)
    {
        wifiConnectTask.enable();
        wifiMonitorTask.disable();
    }
}