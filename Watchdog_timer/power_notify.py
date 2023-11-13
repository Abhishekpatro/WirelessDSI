import wdt
import time
import sys

def retry(func, param=-1):
    r = 3
    ret = -1
    while ret < 0 and r > 0:
        time.sleep(0.1)
        if param != -1:
            ret = func(param)
        else:
            ret = func()
        r -= 1
    return ret
retry(wdt.setPeriod, 0)  #Disable the watchdog
retry(wdt.setRepowerOnBattery, 0) #Do not power back on if the main power is not present
while True:
    time.sleep(3)
    vIn = retry(wdt.getVin)
    vBattery = retry(wdt.getVbat) 
    vPi = retry(wdt.getVrasp) 
    print( 'Line in: %.3fV;  Battery: %.3fV; Voltage to Pi: %.3fV' % ( vIn, vBattery, vPi ) )
    if 0.0 <= vIn < 3.0 and 0.0 <= vBattery < 3.0:
        print("Main power is disconnected and the battery has run down - the system is shutting down!")
        retry(wdt.setPeriod, 30)  # update the period with your maximum time to shutdown
        from subprocess import call
        #call("sudo shutdown -h now", shell=True)




