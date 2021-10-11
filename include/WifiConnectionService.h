#ifndef __WIFICONNECTIONSERVICE_H__
#define __WIFICONNECTIONSERVICE_H__

#define _TASK_OO_CALLBACKS

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

enum class WifiConnectionServiceState;

class WifiConnectionService : public Task
{
    public:
        WifiConnectionService(Scheduler* aScheduler);
        void Init(String hostname);
        bool IsConnected();
        bool Callback();
    private:
        WifiConnectionServiceState state;
        bool stateTransition(uint8_t wifiStatus);
        void tryConnect();
        void monitor();
};


#endif
