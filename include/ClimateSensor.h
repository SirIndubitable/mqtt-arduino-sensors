#ifndef __CLIMATESENSOR_H__
#define __CLIMATESENSOR_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include "DHT.h"

class ClimateSensor : public Task
{
    public:
        ClimateSensor(Scheduler* aScheduler, uint8_t pin, uint8_t dhtType);
        void Init();
        bool Callback();
    private:
        DHT tempSensor;
};

#endif