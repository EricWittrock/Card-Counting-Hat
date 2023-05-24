import subprocess
from pygame import mixer
from time import sleep

mixer.init(44100, -16, 2, 2048)

chirp = mixer.Sound("./audio/chirp1.wav")
chirp.set_volume(0.1)

chirp2 = mixer.Sound("./audio/bet100.wav")
chirp2.set_volume(0.1)


decks = 1

deck = []

for i in range(0,52*decks):
    card = i%13+1
    if card > 10: card = 10
    deck.append(card)


def runProcess(param):
    s = subprocess.check_output("./a.out "+param, stdin=subprocess.PIPE, shell=True)
    return s.decode("utf-8")

def makeDeckKeys():
    keys = [0]*11
    for i in deck:
        keys[i-1]+=1
    if(keys[9] > 10):
        keys[10] = int(keys[9]/10)
        keys[9] = keys[9]%10
    strn=""
    for i in keys:
        strn += str(i) + " "
    strn = strn.strip()
    return strn

def getBet():
    bet = runProcess(makeDeckKeys())
    return float(bet.split("bet: ")[1])

def getDec(dcard, pcards):
    strn = " " + str(dcard)
    for card in pcards:
        strn += (" "+str(card))

    print((makeDeckKeys()+strn))
          
    
    return runProcess(makeDeckKeys()+strn)


print(getBet())
print(getDec(3,[10,10]))

# print(runProcess("1 0 3 0 0 1 1 3 9 5 0 1 2 3"))
