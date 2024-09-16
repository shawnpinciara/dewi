import keyboard
import mido #install: python-rtmidi , mido
#import mido.backends.portmidi  #https://mido.readthedocs.io/en/stable/freezing_exe.html
import time
import os
import platform

win = False
if (platform.system()[0]=='W'): win=True
def clearShell():
    global win
    if win: os.system('cls') #clear shell with shell command for windows
    else: os.system('clear')

###                   USER PARAMETERS                 ###
print("Choose virtual OUT MIDI from these ports: ")
print(mido.get_output_names()) #stampa le porte midi sotto forma di array
print("")
midi_out_index = int(input("Specify index:"))
port = mido.open_output(name=mido.get_output_names()[midi_out_index]) #seleziona la prima porta della lista
print("")
wait:float = float(input("Choose active wait after each press (in milliseconds): "))/1000 #in seconds: 0.01s -> 10 millis?
print(wait)
clearShell()
###                   USER PARAMETERS END             ###

msg_on = mido.Message('note_on')
msg_off = mido.Message('note_off')

#https://gist.github.com/devxpy/063968e0a2ef9b6db0bd6af8079dad2a
NOTES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
OCTAVES = list(range(11))
NOTES_IN_OCTAVE = len(NOTES)
def number_to_note(number: int):
    octave = number // NOTES_IN_OCTAVE
    assert octave in OCTAVES
    assert 0 <= number <= 127
    note = NOTES[number % NOTES_IN_OCTAVE]
    return str(note)+str(octave)


key = 0b0000000000000000
def getButtonsState():
    global key
    #note
    if keyboard.is_pressed("f"):
        key = key | (0b1<<0)
    else:
        key = key & (~(0b1<<0))

    if keyboard.is_pressed("d"):
        key = key | (0b1<<1)
    else:
        key = key & (~(0b1<<1))

    if keyboard.is_pressed("s"):
        key = key | (0b1<<2)
    else:
        key = key & (~(0b1<<2))

    if keyboard.is_pressed("a"):
        key = key | (0b1<<3)
    else:
        key = key & (~(0b1<<3))

    #key
    if keyboard.is_pressed("h"):
        key = key | (0b1<<4)
    else:
        key = key & (~(0b1<<4))
    if keyboard.is_pressed("j"):
        key = key | (0b1<<5)
    else:
        key = key & (~(0b1<<5))
    if keyboard.is_pressed("k"):
        key = key | (0b1<<6)
    else:
        key = key & (~(0b1<<6))
    if keyboard.is_pressed("l"):
        key = key | (0b1<<7)
    else:
        key = key & (~(0b1<<7))

    #octave
    if keyboard.is_pressed("b"):
        key = key | (0b1<<8)
    else:
        key = key & (~(0b1<<8))
    if keyboard.is_pressed("v"):
        key = key | (0b1<<9)
    else:
        key = key & (~(0b1<<9))
    # if keyboard.is_pressed("c"):
    #     key = key | (0b1<<10)
    # else:
    #     key = key & (~(0b1<<10))
    

    return key

threshold_bottom = 5
def getBreath():
   return 10

def noteOn(c,n,v):
    print("note ON: " + str(n) + " , name: "+ number_to_note(n) )
    msg_on.note = n
    port.send(msg_on)
def noteOff(c,n,v):
    #print("note off: " + str(n))
    msg_off.note = n
    port.send(msg_off)
def PolyphonicKeyPressure(c,n,v):
    a=5 #non fa nulla per ora
    #print("PolyphonicKeyPressure")

noteArray = [0,2,11,4,12,9,5,7,1,3,12,5,16,10,6,8]
keyArray =  [0,2,11,4,0,9,5,7,1,3,0,5,16,10,6,8]
octaveArray = [0,1,-1,0,-2,-1,-3,0,0,0,0,0,0,0,0,0]
octave = 5

velocity = 60
threshold_bottom = 5
threshold_top = 15
breath = 10
mask_key = 0b0000000011110000
mask_note = 0b0000000000001111
mask_octave = 0b0000111100000000
currentNote = 0
currentMpr121 = 0
lastMpr121 = 0
noteToSend = 0
lastNote = 3
currentKey = 0
lastKey = 0
currentOctave = 0
lastOctave = 0
mpr121 = 0
octave = 3
breathAttack = True
breathrelease = False
def millis()->float: return time.monotonic()
def mapp(value, inMin, inMax, outMin, outMax): return outMin + (((value - inMin) / (inMax - inMin)) * (outMax - outMin))
def updateBreath():
    global velocity,threshold_bottom,threshold_top,breath,mask_key,mask_note,mask_octave,currentKey,currentNote,currentOctave,lastKey,lastNote,lastOctave,noteArray,keyArray,octave,breathAttack,breathRelease,noteToSend,currentMpr121,lastMpr121
    breath = getBreath()
    if (breath > threshold_bottom):
      mpr121 = getButtonsState()
      currentMpr121 = mpr121
                                         #key handling
      
      if (currentMpr121 != lastMpr121):
        clearShell()
                                            # wait for X millis and get button state
        global wait
        t0 = millis()
        while (millis() - t0)<wait: ##busywaiting##
            mpr121 = getButtonsState()
        
        currentNote = mpr121 & mask_note
        #lastKey = currentKey
        currentKey = (mpr121 & mask_key)>>4
        if (currentKey == 0): 
            currentKey = lastKey
        currentOctave = (mpr121 & mask_octave)>>8
        #cc = (mpr121 & mask_cc) >>11
        velocity = mapp(breath,threshold_bottom,threshold_top,40,127)
        noteOffToSend = ((octave+octaveArray[currentOctave])*12)+(noteArray[lastNote]+keyArray[currentKey])
        noteOff(0,noteOffToSend,velocity)
        lastNote = currentNote
        lastMpr121 = currentMpr121
        lastKey = currentKey
        noteOnToSend = ((octave+octaveArray[currentOctave])*12)+(noteArray[currentNote]+keyArray[currentKey])
        noteOn(0,noteOnToSend, velocity)
        print("")
        print("Octave: " +bin(currentOctave)[2:].zfill(4)+ " Note: " +bin(currentNote)[2:].zfill(4)+" Key: " +bin(currentKey)[2:].zfill(4))
        #print(bin(mpr121)[2:].zfill(16))
        print("")
        print("###### Press Q to quit #####")
      else:
        #noteToSend = ((octave+octaveArray[currentOctave])*12)+(noteArray[currentNote]+keyArray[currentKey])
        PolyphonicKeyPressure(0, currentNote, velocity)
    else: 
      noteOff(0,currentNote,velocity)


while True:
    updateBreath()

    if keyboard.is_pressed("q"):
        print("QUITTING")
        break