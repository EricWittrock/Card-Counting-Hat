import io
import picamera
import logging
import SocketServer as socketserver
from threading import Condition
#from BaseHTTPServer import HTTPServer
import BaseHTTPServer as server
import time
import RPi.GPIO as GPIO
import subprocess
from pygame import mixer

print("run")

mixer.init(44100, -16, 2, 2048)

chirp = mixer.Sound("./audio/chirp1.wav")
chirp.set_volume(0.1)

chirp2 = mixer.Sound("./audio/chirp2.wav")
chirp2.set_volume(0.1)

voice = {}
voice["stand"] = mixer.Sound("./audio/stand.wav")
voice["hit"] = mixer.Sound("./audio/hit.wav")
voice["double"] = mixer.Sound("./audio/double_down.wav")
voice["split"] = mixer.Sound("./audio/split.wav")
voice["surrender"] = mixer.Sound("./audio/surrender.wav")

for i in voice:
    voice[i].set_volume(0.3)

for i in range(0,21):
    strn = str(i*5)
    sound = mixer.Sound("./audio/bet"+strn+".wav")
    sound.set_volume(0.4)
    voice[strn] = sound


decks = 1

deck = []

def error(strn):
    print("an error has occured")
    print(strn)
    exit(1)


def shuffle():
    global deck
    deck = []
    for i in range(0,52*decks):
        card = i%13+1
        if card > 10: card = 10
        deck.append(card)

shuffle()

def removeCards(deck_, cards):
    for card in cards:
        try:
            deck_.remove(card)
        except:
            print("card ("+str(card)+") is not in deck. Cant be removed. The deck is:")
            print(deck_)
        

def runProcess(param):
    s = subprocess.check_output("./a.out "+param, stdin=subprocess.PIPE, shell=True)
    return s.decode("utf-8")

def makeDeckKeys(deck_ = None):
    global deck
    if deck_ is None:
        dk_ = deck
    else:
        dk_ = deck_
    keys = [0]*11
    for i in dk_:
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
    print("bet:")
    print(float(bet.split("bet: ")[1]))
    return float(bet.split("bet: ")[1])


def getDec(dcard, pcards):
    global deck

    deckCopy = deck[:]
    removeCards(deckCopy, pcards)
    removeCards(deckCopy, [dcard])
    
    strn = " " + str(dcard)
    for card in pcards:
        strn += (" "+str(card))
    return runProcess(makeDeckKeys(deckCopy)+strn)


def sayBet():
    bet = str(int(round(getBet()*20)*5))
    print("saying bet: "+bet)
    voice[bet].play()
    

getting = 0
def getCards():
    global getting
    getting = 1

def gotCards(pcards_, dcards_):
    global getting
    print("Got Cards")
    print(pcards_)
    print(dcards_)
    getting = 0
    if len(dcards_) > 1:
        global deck
        print("all cards on table. store these cards and clear mem")
        removeCards(deck, pcards_)
        removeCards(deck, dcards_)
        chirp2.play()
        sayBet()
    else:
        print("Getiing decision")
        decision = getDec(dcards_[0], pcards_)
        print("DECISON")
        print(decision)
        voice[decision].play()
        

GPIO.setmode(GPIO.BCM)

GPIO.setup(26 ,GPIO.OUT)
GPIO.output(26, GPIO.HIGH)

GPIO.setup(19 ,GPIO.IN)
GPIO.setup(13 ,GPIO.IN)
GPIO.setup(6 ,GPIO.IN)

PAGE="""\
<html>
<head>

<title>Raspberry Pi2</title>
</head>
<body>
<center><img src="stream.mjpg"></center>
</body>
</html>
"""

buttons = [0,0,0]

def listenPress():
    global buttons
    if GPIO.input(19) == GPIO.HIGH:
        if buttons[0] == 0:
            chirp.play()
            print("press 0 | get cards ")
            getCards()
        buttons[0] = 1
    if GPIO.input(13) == GPIO.HIGH:
        if buttons[1] == 0:
            chirp.play()
            sayBet()
            print("press 1 || say bet")
        buttons[1] = 1
    if GPIO.input(6) == GPIO.HIGH:
        if buttons[2] == 0:
            chirp.play()
            print("press 2 | shuffling")
            shuffle()
        buttons[2] = 1

    if GPIO.input(19) == GPIO.LOW:
        buttons[0] = 0
        if buttons[0] > 0:
            print("unpress 0")
    if GPIO.input(13) == GPIO.LOW:
        buttons[1] = 0
        if buttons[1] > 0:
            print("unpress 1")
    if GPIO.input(6) == GPIO.LOW:
        buttons[2] = 0
        if buttons[2] > 0:
            print("unpress 2")

def reset():
    global buttons
    buttons[0] = 0
    buttons[1] = 0
    buttons[2] = 0

def stringButtons():
    global buttons
    strn = str(buttons[0])+","+str(buttons[1])+","+str(buttons[2])
    return strn


class StreamingOutput(object):
    def __init__(self):
        self.frame = None
        self.buffer = io.BytesIO()
        self.condition = Condition()

    def write(self, buf):
        if buf.startswith(b'\xff\xd8'):
            # New frame, copy the existing buffer's content and notify all
            # clients it's available
            self.buffer.truncate()
            with self.condition:
                self.frame = self.buffer.getvalue()
                self.condition.notify_all()
            self.buffer.seek(0)
        return self.buffer.write(buf)

class StreamingHandler(server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.send_response(301)
            self.send_header('Location', '/index.html')
            self.end_headers()
        elif self.path == '/index.html':
            content = PAGE.encode('utf-8')
            self.send_response(200)
            self.send_header('Content-Type', 'text/html')
            self.send_header('Content-Length', len(content))
            self.end_headers()
            self.wfile.write(content)
        elif self.path == '/stream.mjpg':
            self.send_response(200)
            self.send_header('Age', 0)
            self.send_header('Cache-Control', 'no-cache, private')
            self.send_header('Pragma', 'no-cache')
            self.send_header('Content-Type', 'multipart/x-mixed-replace; boundary=FRAME')
            self.end_headers()
            try:
                while True:
                    listenPress()
                    with output.condition:
                        output.condition.wait()
                        frame = output.frame
                    self.wfile.write(b'--FRAME\r\n')
                    self.send_header('Content-Type', 'image/jpeg')
                    self.send_header('Content-Length', len(frame))
                    self.end_headers()
                    self.wfile.write(frame)
                    self.wfile.write(b'\r\n')


            except Exception as e:
                logging.warning(
                    'Removed streaming client %s: %s',
                    self.client_address, str(e))
        elif self.path == '/data.txt':
            global getting
            content = str(getting)
            getting = 0
            self.send_response(200)
            self.send_header('Content-Type', 'text')
            self.send_header('Content-Length', len(content))
            self.end_headers()
            self.wfile.write(content)
        elif self.path.find("send:") != -1:
            content = self.path.split("send:")[1]
            arr = content.split(",")
            pcards_ = []
            dcards_ = []
            which = False
            for i in arr:
                if i == '0':
                    which = True
                    continue
                if which:
                    dcards_.append(int(i))
                else:
                    pcards_.append(int(i))

            print("dcard len: " + str(len(dcards_)))
            print("pcard len: " + str(len(pcards_)))

            if len(dcards_) == 0 or len(pcards_) == 0:
                print("not enough cards detected")
                for i in range(0,3):
                    chirp2.play()
                    time.sleep(1)
            else:
                gotCards(pcards_, dcards_)
            
            self.send_response(200)
            self.send_header('Content-Type', 'text')
            self.send_header('Content-Length', len(content))
            self.end_headers()
            self.wfile.write(content)
        else:
            self.send_error(404)
            self.end_headers()

class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
    allow_reuse_address = True
    daemon_threads = True

with picamera.PiCamera(resolution='1920x1080', framerate=12) as camera:
    output = StreamingOutput()
    
    #Uncomment the next line to change your Pi's Camera rotation (in degrees)
    #camera.rotation = 90
    camera.start_recording(output, format='mjpeg')

    
    try:
        address = ('', 8000)
        server = StreamingServer(address, StreamingHandler)
        server.serve_forever()
        
    finally:
        camera.stop_recording()
