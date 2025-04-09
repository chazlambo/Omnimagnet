import math

class ARM:
    pose = None
    temp = None
    calibrate = False
    mapping = False
    
    pose_read = None
    temp_read = None
    
    signalLimits = [] #in order of base>vertical>horizontal>wrist
    mechLimits = []   #in order of slew>height>jut>swivel
    
    def __init__(self, ADC_pose, ADC_temp):
        self.pose_read = ADC_pose
        self.temp_read = ADC_temp
            
    def SetLimits(self, baseMin, baseMax, vertMin, vertMax, horzMin, horzMax, wristMin, wristMax):
        self.signalLimits = [baseMin, baseMax, vertMin, vertMax, horzMin, horzMax, wristMin, wristMax]
        self.calibrate = True
        self.mapping = False
                
    def SetMapping(self, slewMin, slewMax, heightMin, heightMax, jutMin, jutMax, swivelMin, swivelMax):
        self.mechLimits = [slewMin, slewMax, heightMin, heightMax, jutMin, jutMax, swivelMin, swivelMax]
        self.mapping = True
                    
    def mapFun(self, x, inputMin, inputMax, outputMin, outputMax):
        return (x - inputMin) * (outputMax-outputMin) + outputMin
    
    def GetPose(self): # TODO
        if not self.calibrate:
            print("Arm is not calibrated. Set the limits")
            return None
        if not self.mapping:
            print("Missing mapping limits.")
            return None
        
        HT = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]
        theta1 = self.mapFun(self.pose_read[0], self.signalLimits[0], self.signalLimits[1], self.mechLimits[0], self.mechLimits[1])
        d1 = self.mapFun(self.pose_read[1], self.signalLimits[2], self.signalLimits[3], self.mechLimits[2], self.mechLimits[3])
        d3 = self.mapFun(self.pose_read[2], self.signalLimits[4], self.signalLimits[5], self.mechLimits[4], self.mechLimits[5])
        theta4 = self.mapFun(self.pose_read[3], self.signalLimits[6], self.signalLimits[7], self.mechLimits[6], self.mechLimits[7])
        
        HT[0][0] = math.cos(theta1)*math.cos(theta4) - math.sin(theta1)*math.sin(theta4)
        HT[0][1] = -math.cos(theta1)*math.sin(theta4) - math.sin(theta1)*math.cos(theta4)
        HT[0][3] = d3*math.cos(theta1)
        HT[1][0] = math.sin(theta1)*math.sin(theta4) + math.cos(theta1)*math.sin(theta4)
        HT[1][1] = -math.sin(theta4)*math.sin(theta1) + math.cos(theta1)*math.cos(theta4)
        HT[1][3] = d3*math.sin(theta1)
        HT[2][2] = 1
        HT[2][3] = d1
        HT[3][2] = 1

        return HT
                        
    def GetTemp(): #TODO
        return None