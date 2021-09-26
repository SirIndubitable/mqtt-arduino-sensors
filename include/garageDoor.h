#ifdef GARAGE_DOOR_SENSOR
#ifndef __GARAGE_DOOR_H__
#define __GARAGE_DOOR_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

enum class GargeSensorState;

class GarageSensor : public Task
{
    public:
        GarageSensor(Scheduler* aScheduler, uint32_t open_limit_pin, uint32_t close_limit_pin);
        void init();
        bool Callback();
    private:
        GargeSensorState state;
        String topic;
        uint32_t open_limit_pin;
        uint32_t close_limit_pin;
        GargeSensorState get_new_state();
};

#endif
#endif