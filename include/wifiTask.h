#ifndef __WIFITASK_H__
#define __WIFITASK_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

enum class WifiTaskState;

class WifiTask : public Task
{
    public:
        WifiTask(Scheduler* aScheduler);
        void Init(String hostname);
        bool IsConnected();
        bool Callback();
    private:
        WifiTaskState state;
        bool stateTransition(uint8_t wifiStatus);
        void tryConnect();
        void monitor();
};


#endif
