The following mqtt config topics and state topics are what would be created whith the following defines
```
GARAGE_DOOR_SENSOR
PUBLISH_WIFI_RSSI
TEMPERATURE_SENSOR
SENSOR_UNIQUE_NAME="example-name"
```

# Garage Door Sensor

homeassistant/binary-sensor/example-name-open/config
```json
{
    "name": "Garage Door Open",
    "dev_cla": "garage_door",
    "stat_t": "homeassistant/binary-sensor/example-name-gdoor/state",
    "val_tpl": "{{ value_json.garage_door }}",
}
```

homeassistant/binary-sensor/example-name-moving/config
```json
{
    "name": "Garage Door Moving",
    "dev_cla": "moving",
    "stat_t": "homeassistant/binary-sensor/example-name-gdoor/state",
    "val_tpl": "{{ value_json.moving }}",
}
```

homeassistant/binary-sensor/example-name-gdoor/state
```json
{
    "moving": true,
    "garage_door": true
}
```


# Temp/Humidity sensor

homeassistant/sensor/example-name-temp/config
```json
{
    "name": "Temperature",
    "dev_cla": "temperature",
    "stat_t": "homeassistant/sensor/example-name-climate/state",
    "unit_of_meas": "°F",
    "val_tpl": "{{ value_json.temp }}"
}
```

homeassistant/sensor/example-name-humid/config
```json
{
    "name": "Humidity",
    "dev_cla": "humidity",
    "stat_t": "homeassistant/sensor/example-name-climate/state",
    "unit_of_meas": "%",
    "val_tpl": "{{ value_json.humid }}"
}
```


homeassistant/sensor/example-name-climate/state
```json
{
    "temp": 23.20,
    "humid": 43.70
}
```


# Wifi Sensor

homeassistant/sensor/example-name-wifi/config
```json
{
    "name": "Wifi Strength",
    "dev_cla": "signal_strength",
    "stat_t": "homeassistant/sensor/example-name-wifi/state"
}
```

homeassistant/sensor/example-name-wifi/state
```text
-64
```