#ifdef GARAGE_DOOR_SENSOR
#ifndef __GARAGE_DOOR_H__
#define __GARAGE_DOOR_H__

enum class GargeSensorState
{
    Closed,
    Closing,
    Opened,
    Opening,
    Unknown,
    Initializing
};

class GarageSensor
{
    private:
        GargeSensorState state;
        String topic;
        uint32_t open_limit_pin;
        uint32_t close_limit_pin;
        GargeSensorState get_new_state();
    public:
        GarageSensor(uint32_t open_limit_pin, uint32_t close_limit_pin);
        void init();
        void run();
};

#endif
#endif