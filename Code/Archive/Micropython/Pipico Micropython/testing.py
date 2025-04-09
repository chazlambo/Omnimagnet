import machine
import time
from pcf8591 import PCF8591
sda=machine.Pin(0)
scl=machine.Pin(1)
i2c_pose = machine.I2C(0,sda=sda, scl=scl, freq=399361)
print(i2c_pose)
print(i2c_pose.scan())
sda=machine.Pin(2)
scl=machine.Pin(3)
i2c_temp =machine.I2C(1,sda=sda, scl=scl, freq=399361)
print(i2c_temp.scan())
PC1 = PCF8591(i2c_temp,0x48)
#ADC_POSE_arm1 = PCF8591(i2c_pose,0x48)
#ADC_TEMP_arm1 = PCF8591(i2c_temp,0x48)
#ARM arm1 (PCF8591(i2c_pose,0x48), PCF8591(i2c_temp,0x48))

PC1.write(100)
print(PC1.read(0))
time.sleep(0.3)
PC1.write(0)
print(PC1.read(0))

testpin = 2;
arm1 = 0;
arm2 = 0;
arm3 = 0;
arm4 = 0;
arm5 = 0;
for i in range(1,50):
#     print("Pin ", testpin, " - ",i)
    print("Horz Pot")
    PC1.write(0)
    PC2.write(0)
    PC3.write(0)
    PC4.write(0)
    PC5.write(0)
    time.sleep(0.1)
    
    arm1 = arm1 + PC1.read(testpin)
    arm2 = arm2 + PC2.read(testpin)
    arm3 = arm3 + PC3.read(testpin)
    arm4 = arm4 + PC4.read(testpin)
    arm5 = arm5 + PC5.read(testpin)
    


    #voltage = 5.0*PC1.read(0)/255.0
    #print(voltage)
    PC1.write(255)
#     time.sleep(0.1)
    PC2.write(255)
#     time.sleep(0.1)
    PC3.write(255)
#     time.sleep(0.1)
    PC4.write(255)
#     time.sleep(0.1)
    PC5.write(255)
    
    time.sleep(0.1)
    
    print(arm1/i)
    print(arm2/i)
    print(arm3/i)
    print(arm4/i)
    print(arm5/i)
