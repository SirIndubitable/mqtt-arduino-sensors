#ifndef __GARAGE_DOOR_H__
#define __GARAGE_DOOR_H__

enum class GargeSensorState
{
    Closed,
    Closing,
    Opened,
    Opening,
    Initializing
};

class GarageSensor
{
    private:
        GargeSensorState state;
    public:
        GarageSensor();
        void run();
};

#endif