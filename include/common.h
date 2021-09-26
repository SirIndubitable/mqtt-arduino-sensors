#ifndef __COMMON_H__
#define __COMMON_H__

#include <Arduino.h>
#include <PubSubClient.h>

#define DEBUG false  //set to true for debug output, false for no debug output
#define DEBUG_SERIAL if (DEBUG) Serial

#define DEBUG_TIME() \
        { \
            uint32_t elapsed = millis(); \
            DEBUG_SERIAL.print(elapsed / 1000); \
            DEBUG_SERIAL.print("."); \
            DEBUG_SERIAL.print(elapsed % 1000); \
            DEBUG_SERIAL.print(": "); \
        }

extern PubSubClient mqttClient;
extern String baseTopic;

#endif