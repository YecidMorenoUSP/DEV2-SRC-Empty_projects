from rehamove import *         
import time
import numpy as np

r = Rehamove("COM13")

while r.rehamove == None: 
	r = Rehamove("COM4")
	time.sleep(0.5)

t1 = time.time()

tcur = 0
maxmA = 20

freqEst = 180
freqD = (1/8)*2*np.pi;
maVals = []
tVals = []
while(tcur < 4):
        tcur = time.time()-t1
        valmA = np.sin(tcur*freqD) * maxmA
        if(abs(valmA)>maxmA):
                valmA = np.sign(valmA)*maxmA
        r.pulse("blue", valmA, freqEst);
        maVals.append(valmA)
        tVals.append(tcur)
        #time.sleep(1)

print([tVals,maVals])
