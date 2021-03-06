# Domos satellite

This module is an arduino Mega with severals components that are controlled via MQTT

## Dependencies

* LiquidCrystal I2C by Frank de Brabander 1.1.2
* rc-switch by sui77
* Ethernet2 by Various
* `git@github.com:finitespace/BME280.git` (git clone on `~/snap/arduino/41/Arduino/libraries`)
* `git@github.com:256dpi/arduino-mqtt.git`

## Subscriptions

`domos/arduino/#`

### LCD

* `domos/arduino/lcd/0`
* `domos/arduino/lcd/1`
* `domos/arduino/lcd/2`
* `domos/arduino/lcd/3`
* `domos/arduino/lcd/bl`
  * `on`
  * `off`

### Power

* `domos/arduino/power/0`
* `domos/arduino/power/1`
  * `on`
  * `off`

### Octocoupler

* `domos/arduino/octocoupler/0`
* `domos/arduino/octocoupler/1`

### Individual pin

* `domos/arduino/pin/0`
  * `on`
  * `off`
  > It actually is pin 6

## Publish

### Buttons

`domos/arduino/info/button`

> Values 0 to 19

### Meteo

Automatically updated every 5 minutes.

`domos/arduino/info/meteo`

> `{"temperature":22.18,"humidity":57.72,"pressure":1014.72}`
