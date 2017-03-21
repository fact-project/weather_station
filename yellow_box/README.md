# RG11 Rain sensor readout Box

MAGIC and FACT are collaborating in utilizing RG11 rain sensors as an add on for the existing weather stations at the ORM on la palma.

## Two RG11 sensors - two SUBD connectors
Two RG11 sensors exist, which are physically identical, but have different settings. It is not yet fully clear, which setting works best, or if maybe combining both measurements gives the best results. The sensors are labelled:

 - "RG11_1" and
 - "RG11_2"

The readout box has two corresponding SUBD-9 male connectors:

This side of the box should be connected to the sensor "RG11_1". Let's call this side the "one-side".

One-side:
<img src="https://cloud.githubusercontent.com/assets/8200858/24101400/8144cc04-0d78-11e7-95a9-7ad0621f1fc4.jpg" width="120">

This side of the box should be connected to the sensor "RG11_2". Let's call it the "two-side".

Two-side:
<img src="https://cloud.githubusercontent.com/assets/8200858/24101401/8145f566-0d78-11e7-8015-11604c0e50a1.jpg" width="120">


# How to modify the arduino firmware

In short:

 * unplug all 4 cables from the yellow box.
 * Open the box
 * remove the "two-side".
 * attach the TTL2USB adaptor
 * connect your laptop via a normal USB cable
 * flash the firmware.

What you'll need:

 * [Hardware requirements](#hardware-requirements)
 * [Install Platformio](#platformio) or
 * [Install Avrdude](#install-avrdude)
 * [Find the path to your arduino](#find-the-path-to-your-arduino)

If you want to:

 * [Change the IP address](#how-to-change-the-ip-address-of-the-yellow-box)
 * [Just upload](#just-upload-the-firmware-without-modifications)

## Hardware requirements

In order to flash an (ArduinoEthernet)[https://www.arduino.cc/en/Main/ArduinoBoardEthernet]
you need an TTL to USB adaptor (like this one)[https://www.arduino.cc/en/Main/USBSerial].
Most modern Arduino boards have the USB connector mounted, but on the fairly old ArduinoEthernet, they saved the space ...

In order to mount the adaptor you'll need to open the yellow box and remove the "two-side" (just pull it a bit up), to make some space for the adaptor.

On this image:
<img src="https://cloud.githubusercontent.com/assets/8200858/24101308/1b31a180-0d78-11e7-9fc2-43d13bd86607.jpg" width="400">

One can see the box before we decided to add a connector for "RG11_2" as well. So the "two-side" was empty. One can easily pull it out and access the ArduinoEthernets SPI connector.

This image should help to understand what way around the adaptor needs to be
<img src="https://cloud.githubusercontent.com/assets/8200858/24102478/68231696-0d7c-11e7-970a-ef21587c6d36.jpg" width="400">

**Note: Danger!** Please take care not to power the device from two different power supplies. As soon as the arduino is connected via the TTL2USB adaptor to your laptop, your laptop powers it. Do not power the yellow box at the same time with the 24V power supply via its barrel connector. In the best case nothing bad happens, but it might well destroy something.

## Find the path to your arduino

Then you should find a new "device file" for this arduino, e.g. like: `ls /dev/tty{USB,ACM}*`. Let's say the arduino shows up as `/dev/ttyACM0` in your case.

## Platformio

http://docs.platformio.org/en/stable/what-is-platformio.html

The normal arduino IDE is just crap for any serious attempt of writing software and no fun at all.

 * Install Anaconda (shame, if you haven't yet)
 * Create a python 2.7 enviroment: `conda create --name py27 python=2.7`
 * Activate the python 2.7 environmen: `source activate py27`
 * Install platformio: `pip install platformio`
 * cd to your yellow_box directory: `cd path/to/weather_station/yellow_box`
 * build and upload: `platformio run --target upload --upload-port /dev/ttyACM0`

## Install Avrdude

The minimum required tool to flash the firmware file into the arduinos memory is
called `avrdude`. In case you happen to have either platformio or the ArduinoIDE
installed, you will already have a copy of `avrdude` on your machine.

Otherwise you can install it on Ubuntu via:

    sudo apt-get install avrdude

---


## How to change the IP address of the yellow Box?

Just edit this line:
https://github.com/fact-project/weather_station/blob/master/yellow_box/src/RG11.cpp#L11

and build and flash the firmware.


## Just upload the firmware without modifications

For your convenience a copy of the current firmware was checked into this repo as well. This is unusual since the firmware binaries are artifacts of the build process. However, it can be a hassle to install the tool chain for building atmega firmware.

You'll find the firmware in `.pioenvs/ethernet/firmware.hex`.

To flash it into the arduino you'll need to find out which device the arduino
is mounted as, c.f [Hardware requirements](#hardware-requirements).

To flash the arduino do:

    avrdude -patmega328p -P/dev/<tty_something> -b115200 -D -Uflash:w:/full/path/to/yellow_box/.pioenvs/ethernet/firmware.hex:i