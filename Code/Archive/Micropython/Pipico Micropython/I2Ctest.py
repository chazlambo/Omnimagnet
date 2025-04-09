import machine
import time
from OmniArm import *
from pcf8591 import PCF8591

#Initialize Pins
SDA_POSE = 0;
SCL_POSE = 1;
SDA_TEMP = 2;
SCL_TEMP = 3;

for i in range(0,1000):
    #Initialize Position Channel
    sda=machine.Pin(SDA_POSE)
    scl=machine.Pin(SCL_POSE)
    i2c_pose = machine.I2C(0,sda=sda, scl=scl, freq=400000)

    #Check
    print(i2c_pose)
    print(i2c_pose.scan())

    #Initialize Temperature Channel
    sda=machine.Pin(SDA_TEMP)
    scl=machine.Pin(SCL_TEMP)
    i2c_temp =machine.I2C(1,sda=sda, scl=scl, freq=400000)

    #Check
    print(i2c_temp)
    print(i2c_temp.scan())
    print("\n")
