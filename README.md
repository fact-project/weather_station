# weather_station
This repository contains a collection of scripts and code for a arduino based weather station approach for the FACT Weatherstation as wel as some documentation.


# Hardware

We use an arduino micro controler (Uno or Mega) to read the signals from the several sensors wich are described in the following. The arduino will send the collected raw data via serial to a raspberrPi which will do further computions on the recieved Data, e.g., Wind direction, Dew Point etc.

## Sensors

### Temperatur Sensor: DHT22
The DHT22 is a basic, low-cost digital temperature and humidity sensor. It uses a capacitive humidity sensor and a thermistor to measure the surrounding air, and spits out a digital signal on the data pin (no analog input pins needed)

For details see: https://www.adafruit.com/product/385
Documentation: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
Ardunio Library for the DHT22: https://github.com/adafruit/DHT-sensor-library

The design for our weatherstation is based un this instruction: http://fluuux.de/2012/10/arduino-temperatur-und-luftfeuchtigkeit-mit-dem-dht22-prufen/


### Rainsensor: Hydreon RG 11
Manual: http://rainsensors.com/2015/documents/rg-11_instructions.pdf
Tipping bucket example: http://cactus.io/hookups/weather/rain/hydreon/hookup-arduino-to-hydreon-rg-11-rain-sensor

In tipping bucket mode 0.01mm the Sensor produces a 100ms pulse for each detected bucket switch.

### Rainsensor: Kemo M152
https://www.kemo-electronic.de/de/Haustechnik/Garten/M152-Regensensor-12-V-DC.php

### Windsensors:

Readout code on basis of: https://github.com/chiemseesurfer/arduinoWeatherstation
http://maxoberberger.net/projects/arduino-weatherstation/
datasheets: https://www.argentdata.com/files/80422_datasheet.pdf

#### Anemometer
The Anemometer measures via closing contact. 2.4 km/h windspeed cause the contact to close once per second.

#### Wind Vane:
For the wind vane a a 15k resistor is used resulting in the following directions, 0° points away from the Anemometer:

| Direction / Degrees | Resistance R_i / kOhms| Voltage U_i/ V (U_0=5v, R_0=15k) |
| --- | ---   |--- |
|0    | 33    |3.56|
|22.5 |6.57   |1.63|
|45   |8.2    |1.88|
|67.5 |0.891  |0.31|
|90   |1      |0.34|
|112.5|0.688  |0.24|
|135  |2.2    |0.70|
|157.5|1.41   |0.46|
|180  |3.9    |1.12|
|202.5|3.14   |0.94|
|225  |16     |2.71|
|247.5|14.12  |2.51|
|270  |120    |4.52|
|292.5|42.12  |3.80|
|135  |64.9   |4.16|
|337.5|21.88  |3.09|


voltages = {4.52, 3.80, 4.16, 3.09, 3.56, 1.63, 1.88, 0.31, 0.34, 0.24, 0.70, 0.46, 1.12, 0.94, 2.71, 2.51}

directions = {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5}

From the Resistance you can calculate the expected direction voltage as follows:

U_i = U_0 / (R_0/R_i + 1)

## Pin-Allocation
The following is the pin-allocation for arduino mega

| Pin  | Sensor Name| Pin Mode |
| --- | ---   | ---   |
| 2   | wind_speed | interrupt |
| 3   | RG11_1 | interrupt |
| 4   | RG11_2 | interrupt |
| 5   | M152 | interrupt |
| A0  | winf_direction | analoge |
| 8   | DHT22_0 (onboard) | std. digital |
| 9   | DHT22_1 (external | std. digital |
|10   | DHT22_2 (optional)| std. digital |
|11   | DHT22_3 (optional) | std. digital |
|12   | DHT22_4 (optional) | std. digital |


# Software

## usefull Comandlines

arduino --verify weatherstation.ino

arduino --upload weatherstation.ino

## Required Arduino Software version

## Required Arduno Libraries:

### DHT Library
We used version 1.2.3 from: https://github.com/adafruit/DHT-sensor-library

Installed via the arduino package manager

### PciManager
We used version 2.1.0 from: https://github.com/prampec/arduino-pcimanager

Installed via the arduino package manager

### Arduinojson
We used version 5.6.4 from: https://github.com/bblanchon/ArduinoJson

Installed via the arduino package manager
