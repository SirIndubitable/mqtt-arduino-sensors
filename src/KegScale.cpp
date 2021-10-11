#include <Arduino.h>
#include "common.h"
#include "KegScale.h"

#ifndef SENSOR_UNIQUE_NAME
#define SENSOR_UNIQUE_NAME "example-name"
#endif

#define SCALE_STATE_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-keg/state"

#define SCALE_CONFIG_TOPIC "homeassistant/sensor/" SENSOR_UNIQUE_NAME "-keg/config"
#define SCALE_CONFIG_DATA \
"{ " \
"\"name\": \"" SENSOR_UNIQUE_NAME " Fullness\", " \
"\"stat_t\": \"" SCALE_STATE_TOPIC "\", " \
"\"unit_of_meas\": \"%\", " \
"}"

KegScale::KegScale(
    Scheduler* aScheduler,
    PubSubClient* mqttClient,
    uint32_t data_pin,
    uint32_t clock_pin,
    uint32_t led_pin,
    uint32_t low_cal_pin,
    uint32_t high_cal_pin)
    : MqttSensor(aScheduler, mqttClient), load_scale()
{
    this->led_pin = led_pin;
    this->low_cal_pin = low_cal_pin;
    this->high_cal_pin = high_cal_pin;
    this->data_pin = data_pin;
    this->clock_pin = clock_pin;
}

void KegScale::Init()
{
    pinMode(this->led_pin, PinMode::OUTPUT);
    pinMode(this->low_cal_pin, PinMode::INPUT);
    pinMode(this->high_cal_pin , PinMode::INPUT);
    this->load_scale.begin(this->data_pin, this->clock_pin);
    this->load_scale.wait_ready();
}

uint32_t KegScale::OnMqttConnected()
{
    this->PublishMessage(SCALE_CONFIG_TOPIC, SCALE_CONFIG_DATA);

    return 500 * TASK_MILLISECOND;
}

void KegScale::ReadAndPublish()
{
    digitalWrite(this->led_pin, PinStatus::LOW);
    auto low_cal = digitalRead(this->low_cal_pin) == PinStatus::LOW;
    auto high_cal = digitalRead(this->high_cal_pin) == PinStatus::LOW;
    if (this->last_low_cal_read && low_cal)
    {
        this->calibrate_low();
        digitalWrite(this->led_pin, PinStatus::HIGH);
        DEBUG_TIME();
        DEBUG_SERIAL.print("Offset: ");
        DEBUG_SERIAL.println(this->load_scale.get_offset());
    }
    else if (this->last_high_cal_read && high_cal)
    {
        this->calibrate_high();
        digitalWrite(this->led_pin, PinStatus::HIGH);
        DEBUG_TIME();
        DEBUG_SERIAL.print("Scale: ");
        DEBUG_SERIAL.println(this->load_scale.get_scale());
    }
    else if (this->load_scale.get_scale() != 1.0f && this->load_scale.get_offset() != 0)
    {
        auto value = this->load_scale.get_units();
        if (abs(this->last_read - value) > 1.0)
        {
            this->last_read = value;

            char percent[4];
            itoa((int)round(value), percent, 10);
            this->PublishMessage(SCALE_STATE_TOPIC, percent);
        }
    }
    this->last_low_cal_read = low_cal;
    this->last_high_cal_read = high_cal;
}

void KegScale::calibrate_low()
{
    this->load_scale.tare();
}

void KegScale::calibrate_high()
{
    auto value = this->load_scale.read_average();
    auto offset = this->load_scale.get_offset();

    this->load_scale.set_scale((100 + offset) / value);
}