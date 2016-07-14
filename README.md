# weather_station
This repository contains a collection of scripts and code for a arduino based weather station approach for the FACT Weatherstation as wel as some documentation.


# Hardware

## Temperatur Sensor: DHT22
The DHT22 is a basic, low-cost digital temperature and humidity sensor. It uses a capacitive humidity sensor and a thermistor to measure the surrounding air, and spits out a digital signal on the data pin (no analog input pins needed)

For details see: https://www.adafruit.com/product/385
Documentation: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
Ardunio Library for the DHT22: https://github.com/adafruit/DHT-sensor-library

The design for our weatherstation is based un this instruction: http://fluuux.de/2012/10/arduino-temperatur-und-luftfeuchtigkeit-mit-dem-dht22-prufen/


# Software

## Protokoll Buffers:

In order to transfer data via the serial port to e.g. a raspberry pi we use nanopb: https://github.com/nanopb/nanopb
As maxnoe did for his arduino project (https://github.com/MaxNoe/arduino/tree/master/read_analog_data)

### Requirements:
* Protobuf3
* nanopb: https://github.com/nanopb/nanopb/releases (download and unzip to arduino library folder)
