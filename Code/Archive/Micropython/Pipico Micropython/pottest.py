import machine
import time
from OmniArm import *
from pcf8591 import PCF8591

#Initialize Pins
SDA_POSE = 0;
SCL_POSE = 1;
SDA_TEMP = 2;
SCL_TEMP = 3;

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

#Initialize Position ADC Modules
# PC1 = PCF8591(i2c_pose,0x48) #Arm 1
# PC2 = PCF8591(i2c_pose,0x49) #Arm 2
# PC3 = PCF8591(i2c_pose,0x4C) #Arm 3
# PC4 = PCF8591(i2c_pose,0x4D) #Arm 4
# PC5 = PCF8591(i2c_pose,0x4F) #Arm 5

#Initialize Temperature ADC Modules
PC1 = PCF8591(i2c_temp,0x48) #Arm 1
PC2 = PCF8591(i2c_temp,0x49) #Arm 2
PC3 = PCF8591(i2c_temp,0x4C) #Arm 3
PC4 = PCF8591(i2c_temp,0x4D) #Arm 4
PC5 = PCF8591(i2c_temp,0x4F) #Arm 5

#Loop Variables
sleeptime = 0.5;
iters = 10;
arm_avg = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]];



#Loop Potentiometer Readings
for i in range(1,iters):
    
    #Turn off Green LED
    PC1.write(0)
    PC2.write(0)
    PC3.write(0)
    PC4.write(0)
    PC5.write(0)
    time.sleep(sleeptime)
    
    for j in range(0,4):
        #Recursive Average Calculation
        arm_avg[0][j] = arm_avg[0][j] + PC1.read(j)
        arm_avg[1][j] = arm_avg[1][j] + PC2.read(j)
        arm_avg[2][j] = arm_avg[2][j] + PC3.read(j)
        arm_avg[3][j] = arm_avg[3][j] + PC4.read(j)
        arm_avg[4][j] = arm_avg[4][j] + PC5.read(j)
    
    
    #Turn on Green LED
    PC1.write(255)
    PC2.write(255)
    PC3.write(255)
    PC4.write(255)
    PC5.write(255)
    time.sleep(sleeptime)

for a in range(0,5):
    for b in range(0,4):
        arm_avg[a][b] = arm_avg[a][b]/(i-1);
        arm_avg[a][b] = round(arm_avg[a][b],1);
    print("Arm ",a+1,"\t",arm_avg[a][0],"\t",arm_avg[a][1],"\t",arm_avg[a][2],"\t",arm_avg[a][3],"\n")