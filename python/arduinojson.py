#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import json
import datetime
import numpy as np
import pandas as pd
from pprint import PrettyPrinter

pp = PrettyPrinter()

deg = (0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 135, 337.5)
resistance = (33, 6.57, 8.2, 0.891, 1, 0.688, 2.2, 1.41, 3.9, 3.14, 16, 14.12, 120, 42.12, 64.9, 21.88)
voltage = (3.56, 1.63, 1.88, 0.31, 0.34, 0.24, 0.70, 0.46, 1.12, 0.94, 2.71, 2.51, 4.52, 3.80, 4.16, 3.09)
windDirectionDF = pd.DataFrame(dict(deg=deg, resistance=resistance, voltage=voltage))
windDirectionDF = windDirectionDF.sort("voltage")

def convertVoltage2WindDirection(volt):
    min_div = 100
    deg=0
    for i in windDirectionDF.itertuples():
        div = np.abs(volt - i[3])
        if min_div > div:
            min_div=div
            deg=i[1]
    return deg


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
