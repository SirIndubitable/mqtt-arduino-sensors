This repo stores all of the source for various MQTT sensors.  I intend them to work with HomeAssistant
but they should work for any MQTT service.

# Sensors:
1) Garage door sensor (In work)
    - This sensor connects to the garage door opener limit switches to detect if it is open or closed.
2) Temperature sensor (Not started)
3) Door shut & Lock sensor (Not started)

# Requirements:
1) Arduino Nano 33 iot board (will probably work for other similar boards, but no guarentees)
2) PlatformIO VSCode plugin

# Setup:
1) Update `./include/secrets.h` to include your MQTT information and WIFI login information
2) Call `git update-index --assume-unchanged "./include/secrets.h"` to avoid accidently commiting your secrets
3) Set the environment to your desired sensor (default is garage_door_sensor)

# Debugging
Setting DEBUG to true in `./include/common.h` will enable serial debug print statements