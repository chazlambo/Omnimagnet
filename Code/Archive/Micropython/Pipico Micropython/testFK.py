import machine
import time
from OmniArm import *

armInp = 500
baseInp = 200
horzInp = 300
vertInp =  400

arm1_ADC_pose = [armInp, baseInp, horzInp, vertInp]          #=PCF8591(i2c_pose,0x48)
arm1_ADC_temp = [0,0,0]                                      #=PCF8591(i2c_temp,0x48)
arm1 = ARM(arm1_ADC_pose, arm1_ADC_temp)

baseMin = 1
baseMax = 1000
vertMin = 2
vertMax = 2000
horzMin = 3
horzMax = 3000
armMin = 4
armMax = 4000


arm1.SetLimits(baseMin, baseMax, vertMin, vertMax, horzMin, horzMax, armMin, armMax)


slewMin= 0
slewMax= 2*math.pi
heightMin= 0.2
heightMax= 0.4
jutMin= 0.1
jutMax= 0.5
swivelMin= 0
swivelMax= 2 * math.pi

arm1.SetMapping(slewMin, slewMax, heightMin, heightMax, jutMin, jutMax, swivelMin, swivelMax)
