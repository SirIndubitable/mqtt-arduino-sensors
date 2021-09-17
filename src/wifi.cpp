#include "common.h"
#include "secrets.h"

void tryConnectWifi();
void monitorWifi();

Task wifiConnectTask(10 * TASK_SECOND, TASK_FOREVER, tryConnectWifi, &runner, true);
Task wifiMonitorTask(30 * TASK_SECOND, TASK_FOREVER, monitorWifi,    &runner, false);

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

bool wifi_state_transition()
{
    wl_status_t status = (wl_status_t)WiFi.status();

    if (status == wl_status_t::WL_NO_MODULE)
    {
        DEBUG_TIME();
        DEBUG_SERIAL.print("ERROR - No Wifi Module");
        wifiConnectTask.disable();
        wifiMonitorTask.disable();
        return true;
    }
    if (status == wl_status_t::WL_CONNECTED)
    {
        wifiConnectTask.disable();
        return !wifiMonitorTask.enableIfNot();
    }

    wifiMonitorTask.disable();
    return !wifiConnectTask.enableIfNot();
}

void tryConnectWifi()
{
    DEBUG_TIME();
    DEBUG_SERIAL.println("Connecting to Wifi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (wifi_state_transition())
    {
        return;
    }

    DEBUG_TIME();
    DEBUG_SERIAL.print("ERROR - ");
    DEBUG_SERIAL.println(status_tostring((wl_status_t)WiFi.status()));
}

void monitorWifi()
{
    if (wifi_state_transition())
    {
        DEBUG_TIME();
        DEBUG_SERIAL.println(status_tostring((wl_status_t)WiFi.status()));
    }
    #ifdef PUBLISH_WIFI_RSSI
    else
    {
        String rssi_topic = baseTopic + "/wifi/rssi";
        char rssi[12];
        itoa(WiFi.RSSI(), rssi, 10);
        DEBUG_SERIAL.print("Wifi RSSI: ");
        DEBUG_SERIAL.print(rssi);
        DEBUG_SERIAL.println(" dBm");
        mqttClient.publish(rssi_topic.c_str(), rssi);
    }
    #endif
}