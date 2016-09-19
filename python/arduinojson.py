#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import json
import datetime
import numpy as np
import pandas as pd
from pprint import PrettyPrinter

pp = PrettyPrinter()

def main():
    count = 0
    ser = serial.Serial("/dev/ttyACM0",9600,timeout=1)

    pp.pprint("Startup")
    with open('/home/pi/mnt/weather_data/data.json', 'a') as outfile:
        while True:
            if ser.inWaiting()>0:
                """Get a line of data, strip out the null characters and new lines"""
                data = str(ser.readline()).replace("b\'","").replace("\\r\\n\'","")
                d = False
                try:
                    d = json.loads(data)
                except (KeyboardInterrupt, SystemExit):
                    raise
                except:
                    print("Data not readable")
                if d:
                    d["time_stamp"] = datetime.datetime.utcnow().isoformat()
                    d["windDirectionDeg"] = convertVoltage2WindDirection(d["windDirection"]/1000.0)
                    pp.pprint(d)
                    count = count + 1
                    json.dump(d, outfile)
                    outfile.write('\n')

if __name__ == "__main__":
	main()
