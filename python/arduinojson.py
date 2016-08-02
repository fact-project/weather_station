#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import json
import datetime
import numpy as np
from pprint import PrettyPrinter

pp = PrettyPrinter()

def main():
    count = 0
    ser = serial.Serial("/dev/ttyACM0",9600,timeout=1)

    pp.pprint("Startup")
    while (True):
        if (ser.inWaiting()>0):
            """Get a line of data, strip out the null characters and new lines"""
            data = str(ser.readline()).replace("b\'","").replace("\\r\\n\'","")
            try:
                d = json.loads(data)
                d["time_stamp"] = datetime.datetime.utcnow()
			
                pp.pprint(d)
		print "\n"
                count = count + 1
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                print("Data not readable")

if __name__ == "__main__":
	main()
