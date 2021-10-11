#ifndef __KEG_SCALE_H_HH
#define __KEG_SCALE_H_HH

#include "MqttSensor.h"
#include "HX711.h"

class KegScale : public MqttSensor
{
    public:
        KegScale(
            Scheduler* aScheduler,
            PubSubClient* mqttClient,
            uint32_t data_pin,
            uint32_t clock_pin,
            uint32_t led_pin,
            uint32_t low_cal_pin,
            uint32_t high_cal_pin);
        void Init();
    protected:
        uint32_t OnMqttConnected();
        void ReadAndPublish();
    private:
        void calibrate_low();
        void calibrate_high();
        HX711 load_scale;
        uint32_t led_pin;
        uint32_t low_cal_pin;
        uint32_t high_cal_pin;
        uint32_t data_pin;
        uint32_t clock_pin;
        bool last_high_cal_read;
        bool last_low_cal_read;
        float last_read;
};

#endif
