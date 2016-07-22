#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import json
import datetime
import numpy as np

def main():
    count = 0
    ser = serial.Serial("/dev/ttyACM2",9600,timeout=1)

    while (True):
        if (ser.inWaiting()>0):
            """Get a line of data, strip out the null characters and new lines"""
            data = str(ser.readline()).replace("b\'","").replace("\\r\\n\'","")
            try:
                d = json.loads(data)
                d["time"] = datetime.datetime.utcnow()
                print(d)
                count = count + 1
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                print("Data not readable")

if __name__ == "__main__":
	main()
