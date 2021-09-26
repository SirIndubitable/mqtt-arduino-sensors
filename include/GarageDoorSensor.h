#ifndef __GARAGE_DOOR_H__
#define __GARAGE_DOOR_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

enum class GarageDoorSensorState;

class GarageDoorSensor : public Task
{
    public:
        GarageDoorSensor(Scheduler* aScheduler, uint32_t open_limit_pin, uint32_t close_limit_pin);
        void init();
        bool Callback();
    private:
        GarageDoorSensorState state;
        String topic;
        uint32_t open_limit_pin;
        uint32_t close_limit_pin;
        GarageDoorSensorState get_new_state();
};

#endif
