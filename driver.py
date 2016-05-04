#!/usr/bin/env python

import serial, time

serial = serial.Serial("/dev/ttyUSB0", 115200)
time.sleep(3)
command = 0x02 #Wheel CW
data = 0x0A
byte = ((command << 4) | data).to_bytes(1, byteorder="big")
serial.write(byte)
time.sleep(2)
serial.close()
