# weather_station
This repository contains a collection of scripts and code for a arduino based weather station approach for the FACT Weatherstation as wel as some documentation.


# Hardware

## Temperatur Sensor: DHT22
The DHT22 is a basic, low-cost digital temperature and humidity sensor. It uses a capacitive humidity sensor and a thermistor to measure the surrounding air, and spits out a digital signal on the data pin (no analog input pins needed)

For details see: https://www.adafruit.com/product/385
Documentation: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
Ardunio Library for the DHT22: https://github.com/adafruit/DHT-sensor-library

The design for our weatherstation is based un this instruction: http://fluuux.de/2012/10/arduino-temperatur-und-luftfeuchtigkeit-mit-dem-dht22-prufen/


## Rainsensor: Hydreon RG 11
Tipping bucket example: http://cactus.io/hookups/weather/rain/hydreon/hookup-arduino-to-hydreon-rg-11-rain-sensor

In tipping bucket mode 0.01mm the Sensor produces a 100ms pulse for each detected bucket switch.

## Windsensors:

Readout code on basis of: https://github.com/chiemseesurfer/arduinoWeatherstation
http://maxoberberger.net/projects/arduino-weatherstation/
datasheets: https://www.argentdata.com/files/80422_datasheet.pdf

### Anemometer
The Anemometer measures via closing contact. 2.4 km/h windspeed cause the contact to close once per second.

### Wind Vane:
For the wind vane a a 15k resistor is used resulting in the following directions, 0° points away from the Anemometer:

| Direction / Degrees | Resistance / Ohms| Voltage / V (V=5v, R=15k) |
| --- | ---   | --- |
| 0   |       |4.52|
|22.5 |       |3.80|
|45   |       |4.16|
|67.5 |       |3.09|
|90   |       |3.56|
|112.5|       |1.63|
|135  |       |1.88|
|157.5|       |0.31|
|180  |       |0.34|
|202.5|       |0.24|
|225  |       |0.70|
|247.5|       |0.46|
|270  |       |1.12|
|292.5|       |0.94|
|315  |       |2.71|
|337.5|       |2.51|


voltages = {4.52, 3.80, 4.16, 3.09, 3.56, 1.63, 1.88, 0.31, 0.34, 0.24, 0.70, 0.46, 1.12, 0.94, 2.71, 2.51}

directions = {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5}



# Software

### Requirements:
