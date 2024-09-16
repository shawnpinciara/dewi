import keyboard
import time

def millis(): return time.monotonic()
t0 = millis()
wait = float(input("Choose active wait after each press (in milliseconds): "))/1000 #in seconds: 0.01s -> 10 millis?

key = 0b0000
def getButtonsState():
    global key
    if keyboard.is_pressed("a"):
        key = key | 0b0000000000000001
    else:
        key = key & (~0b0000000000000001)

    if keyboard.is_pressed("s"):
        key = key | 0b0000000000000010
    else:
        key = key & (~0b0000000000000010)

    if keyboard.is_pressed("d"):
        key = key | 0b0000000000000100
    else:
        key = key & (~0b0000000000000100)

    if keyboard.is_pressed("f"):
        key = key | 0b1000
    else:
        key = key & (~0b0000000000001000)
    return key

while True:
    t0 = millis()
    state = 0
    state = getButtonsState()
    while (millis() - t0)<wait: ##busywaiting##
        state = getButtonsState()
        #print(millis() - t0)
    print(state)