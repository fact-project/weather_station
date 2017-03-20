### RG11 Arduino

Note:  device file `/dev/ttyACM0` is not unique, access the arduino better by e.g. this:

    /dev/serial/by-id/usb-Arduino__www.arduino.cc__Arduino_USB-Serial_6493633313735151B0D1-if00

or by the symlink Max Noethe has made:

    /dev/lid-arduino

# We are using platformio

http://docs.platformio.org/en/stable/what-is-platformio.html

Simply since we can develop our arduinos remotely an X-server. Since platformio is still only python 2 compliant, we propose to install it like this:

## Install Anaconda (shame, if you haven't yet)

## Create a python 2.7 enviroment

    conda create --name py27 python=2.7

## Activate the python 2.7 environmen, if you've allready created it

    source activate py27

## Install platformio

    pip install platformio


## build and upload with

    cd Software/ShutterController/
    platformio run -t upload

## connect to serial monitor with

    cd Software/ShutterController/
    platformio device monitor -p /dev/lid-arduino -b 115200


# platformio.ini contents:

    [env:ethernet]
    platform = atmelavr
    board = ethernet
    framework = arduino
    upload_port = /dev/lid-arduino

