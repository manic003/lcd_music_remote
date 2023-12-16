import sys
import os 
import threading
import time
from ast import literal_eval  # for converting from string to dict
import datetime
import random
import paho.mqtt.client as mqtt
import logging

from infrared_module import broadlinkYamaha
from smart_functions import utils
from state_observer_module.StateObserver import StateObserver
from music_module.MultiroomMusicControl import EightButtonRemote
from music_module import getSpotifyData, spotify,  noxon_internetradio


remote_yamaha = EightButtonRemote("yamaha",True,"fromLCD")
file = "/home/pi/test_state_lcd.txt"

KILLCHILD = False

def waitForNetflix():
    global KILLCHILD
    KILLCHILD = True
    x = broadlinkYamaha.netflix_scene()

PLAYLISTS = None
try:
    PLAYLISTS = getSpotifyData.get_all_playlists()

except:
    print("FEHLER: Konnte playlistdata nicht abrufen!")

TIMESTAMP = 0
BL_ON = True
TIMESTAMP_LINE3 = 0
TEXT_IN_LINE3  = False


current_playlists = dict(a={"name":"--", "uri":"None"},b={"name":"--", "uri":"None"},c={"name":"--", "uri":"None"}, d={"name":"--", "uri":"None"})




def playIT(key):
    # Plays on Yamaha
    spotify.playlist(name=current_playlists[key]['uri'],yamaha=True)
    print("playling ",current_playlists[key]['uri'])
    set_text(1,0,current_playlists[key]['name'][0:19],True)

def playA():
    playIT("a")

def playB():
    playIT("b")


def playC():
    playIT("c")

def playD():
    playIT("d")



def set_text(line, tab, msg, clear=True):
    """
    sets text to LCD via MQTT

    Syntax:
    - line [0-3] definining in which line text is to be set
    - tab [0-19] defining tab
    - msg  = text to be set on display
    - clear (boolean)  choose weather the whole line is cleared before setting the new text
    if tab + len(msg) greater than 20
    the  display writes into next line!!!


    (the appending of 0/1 and the chars is just my made up protocol to interpret the commands,
    have a look in the cpp Sketch if you want to understand/ need change something)

    """

    global BL_ON, TIMESTAMP
    if len(msg) == 0:
        print("ERROR at least one char needed!")
        return None

    if clear:
        msg += "1"
    else:
        msg+= "0"
    msg += str(line)

    if tab >9:
        msg += chr( tab + ord("0"))
    else:
        msg+= str(tab)
#    print(msg)
    client.publish(topic="/home/LCD/inputESP", payload=msg)
    BL_ON = True
    TIMESTAMP = datetime.datetime.timestamp(datetime.datetime.now())



def write_symbol(line, tab, index, clear=False):
    msg = "$" + str(index)
    set_text(line, tab, msg, clear)

def backlight_off():
    global BL_ON
    client.publish(topic="/home/LCD/inputESP", payload="#&b0")
    BL_ON = False

def backlight_on():
    global BL_ON
    client.publish(topic="/home/LCD/inputESP", payload="#&b1")
    BL_ON = True

def clear_lcd():
   msg = 80*" "
   set_text(0, 0, msg)


dt = datetime
def print_lcd(msg=None):
    """
        print function for the 'Buttons' 
        - schreibt ein Check-Haken-Symbol als Bestaetigung an das LCD in 
          Zeile 3/4 ans Ende der dort dargestellt Button beschreibung
          [Diese wird vom LCD Display selbst geschrieben] UND setzt den Timestamp fuer
          Zeile 3
       -  

    """
    global TIMESTAMP_LINE3, TEXT_IN_LINE3
    if msg is None:
        write_symbol(2,19,4) # Haken..
    else:
        set_text(1,0,msg[0:18],True)

    TIMESTAMP_LINE3 = dt.datetime.timestamp(dt.datetime.now())
    TEXT_IN_LINE3  = True


def choose4():
    global current_playlists
    """ set four random playlists on screen and into current_playlists dict"""
    lenP = len(PLAYLISTS)
    sample = random.sample(range(lenP),4)
    k=0
    for key in current_playlists.keys():

        current_playlists[key]["name"] = PLAYLISTS[sample[k]]["name"]
        current_playlists[key]["uri"] = PLAYLISTS[sample[k]]["uri"]

        k+=1
#    return (current_playlists)
    # TODO eigentlich noch aufs LCD schreiben...
    print("playlists. set")

class RemoteButtons:
    all_buttons = []
    last_received_code = None
    time_stamp = 0
    buttons_pressed_counter = 0

    def receive_code(code):
        RemoteButtons.last_received_code = code
        RemoteButtons.time_stamp = datetime.datetime.timestamp(datetime.datetime.now())

    def __init__(self, name, code, function, description, delay, func_arg=False,print_lcd_msg=False):
        self.name = name
        self.code = code
        self.function = function
        self.description = description
        self.delay = delay
        self.timestamp = 0
        self.func_arg = func_arg
        self.print_lcd_msg = print_lcd_msg
        RemoteButtons.all_buttons.append(self)

    def run_func(self):  # vermeidet das bei mehrmaligem Druecken die Funktion mehrfach ausfuehert wird
        if self.timestamp + self.delay < RemoteButtons.time_stamp:
            self.function()
            self.timestamp = datetime.datetime.timestamp(datetime.datetime.now())
            print(datetime.datetime.now().strftime("%d.%m.%y %H:%M [:%Ssec]"), " executing: ", self.description)
            RemoteButtons.buttons_pressed_counter += 1
        else:
            pass

    def run_func_arg(self):
        if self.timestamp + self.delay < RemoteButtons.time_stamp:
            self.function(self.func_arg)
            self.timestamp = datetime.datetime.timestamp(datetime.datetime.now())
            print(datetime.datetime.now().strftime("%d.%m.%y %H:%M [:%Ssec]"), " executing: ", self.description)
            RemoteButtons.buttons_pressed_counter += 1
        else:
            pass

    def was_pressed(self):
        return self.code == RemoteButtons.last_received_code

    def lcd_output(self):
        if self.print_lcd_msg:
            print_lcd(self.description)
        else:
            print_lcd()


#Mode A Spotify Control
RemoteButtons(" ", "1a", remote_yamaha.play_pause, "{Sony}: spotify PLAY_PAUSE!", 0.8)
RemoteButtons("None", "2a", remote_yamaha.skip, "{Sony}:spotify SKIP !", 0.7)
RemoteButtons(" ", "3a", remote_yamaha.vol_up, "{Sony}: volume UP!", 0.01)
RemoteButtons(" " , "4a", remote_yamaha.vol_down, "{Sony}: volume DOWN!", 0.01)

#Mode B Radio Control
RemoteButtons("ModeRadio B1", "1b", remote_yamaha.spotify_radio_switch, "{Sony}: Radio/Spotify!", 1)
#RemoteButtons(" C ON Channel 1", "pause", remote_yamaha.play_pause, "{Sony}: spotify PLAY_PAUSE!", 0.8)
RemoteButtons("None", "2b", remote_yamaha.skip, "{Sony}:spotify SKIP !", 0.7)

RemoteButtons(" ", "3b", noxon_internetradio.lauter_1, "{Sony}: volume UP!", 0.01)
RemoteButtons("None", "4b", noxon_internetradio.leiser_1, "{Sony}: volume DOWN!", 0.01)

#Mode C Starting one of 4 random Playlist
RemoteButtons("None", "1c", playA, "PlaylistA", 0.7)
RemoteButtons("None",  "2c", playB, "PlaylistB", 0.7)
RemoteButtons("None", "3c", playC, "PlaylistC", 0.7)
RemoteButtons("None", "4c", playD, "PlaylistD", 0.7)


# When switching to Mode3 (C) reload 4 random playlist (a,b,c,d)
RemoteButtons("Playlist select.", "mode3", choose4, "Playlist select:", 0.7)


# create volume remote buttons 0 -20 
for i in range(0,21):
    vol = "{value:02d}".format(value=i)
    RemoteButtons(name="vol setter to:"+ vol, code=vol,function=noxon_internetradio.todo_volume, description="setting volume to:"+vol, delay=0.0, func_arg=int(vol),print_lcd_msg=False)



def set_on_ModeD():
    set_text(1,0,"c)=lichtK d)=lichtW",True)

#Mode D start netflix scene

RemoteButtons("set button descr.", "mode4", set_on_ModeD, "light.:", 0.7)
RemoteButtons("netflix!", "1d" , waitForNetflix, "netflix scene", 2.5)
RemoteButtons("ne!", "3d" ,utils.bed01 , "licht", 2.5)
RemoteButtons("ne!", "4d" , utils.kitchen01, "licht", 2.5)



def check_backlight():
    if (datetime.datetime.timestamp(datetime.datetime.now())- TIMESTAMP >= 20) and BL_ON:
        backlight_off()
        backlight_on = False

def check_msg():
    if (datetime.datetime.timestamp(datetime.datetime.now())- TIMESTAMP >= 1.5) and BL_ON:
#        set_text()
        print("clearing line 1")
        set_text(1,0," ", True)



def on_message(client, userdata, message):
    global KILLCHILD
    msg = str(message.payload.decode("utf-8"))
    print("\t\MQTTt===>",msg, message.topic,"\n")
    if 1 == 1:
#    if message.topic == "/home/LCD/outputToRaspy" :
        #msg_dict = literal_eval(msg)
        received_code = msg
#        print("\t\t\t",received_code)
#        print("is gleich play?:" + str(received_code== "play"))
        RemoteButtons.receive_code(received_code)

        if message.topic == "home/MusicData/Spotify/fromLCD___REMOVE" :
            
            if "Play_Yamaha" in received_code:
                print("is ")
                write_symbol(0,0,0)

            elif "Pause_Yamaha" in received_code:
                    write_symbol(0,0,3)

            set_text(1,0,received_code[0:19])
        else:
            
            for button in RemoteButtons.all_buttons:
                if button.was_pressed():
                    try:
                        print("Tastendrucknr.: ", RemoteButtons.buttons_pressed_counter, end="  ")
                        if button.func_arg:
                            button.run_func_arg()
                        else:
                            button.run_func()
                        button.lcd_output()

                        #TODO was macht das nochmal? noetig??
                        if KILLCHILD:
                            print("killing child!")
                            os.wait()
                            KILLCHILD = False

                    except Exception as E:
                        error_msg = "ERROR:" + str(E)
                        msg = error_msg[0:20]
                        set_text(1,0,msg)
    #                    print_lcd(error_msg)
                        print(error_msg)


                    sys.stdout.flush()




def on_connect(client, userdata, flags, rc):
    client.subscribe('/home/LCD/outputToRaspy')
    client.subscribe("home/MusicData/Spotify/fromLCD")
    now = datetime.datetime.now()

    dt_string = now.strftime("%H:%M")

    set_text(1,0,"connected@"+dt_string, False)


BROKER_ADDRESS = "localhost"
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER_ADDRESS)



####################### TEST OBSERVER ##################



def set_songname(name):
    set_text(1,0,name[0:19],True)
    print("setting song name!")


def switch_1(state):
    if state:
        set_play_symbol()
    else:
        set_pause_symbol()

def set_play_symbol():
    print("writing playsymb)")
    write_symbol(0,0,0)

def set_pause_symbol():
    write_symbol(0,0,3)
    print("writing pause sym")



myObserver = StateObserver()
myObserver.add_listener(StateObserver.STATES.songname, set_songname)
myObserver.add_listener(StateObserver.STATES.yamaha_playing, switch_1)



#playstate_property = myObserver.get_property_of(StateObserver.STATES.yamaha_playing)
#playstate_property.set_action_if_state_equals(set_play_symbol,True)
#playstate_property.set_action_if_state_equals(set_pause_symbol,False)



def main():
    global TEXT_IN_LINE3
    # BROKER_ADDRESS= "ESP8266Client"
    print(10 * "\n")
    print("Welcome to your musicsesrver:")
    print("Connected to MQTT Broker: " + BROKER_ADDRESS)
    print("SKRIPT GESTARTET AM: ", datetime.datetime.now())

    t2 = threading.Thread(target=myObserver.run_listener)
    t2.start()


#    set_text(0,0,"ready")

    while True:
        #check_backlight()
#        check_msg()
#
#        print("now =", dt.datetime.timestamp(dt.datetime.now()))
#        print("stamp = ", TIMESTAMP_LINE3)
#        print("dif=", dt.datetime.timestamp(dt.datetime.now()) - TIMESTAMP_LINE3)
#        print(TEXT_IN_LINE3," \n")
        if TEXT_IN_LINE3 and dt.datetime.timestamp(dt.datetime.now()) - TIMESTAMP_LINE3 > 1.0:
            set_text(2,0,20*" ")
            TEXT_IN_LINE3 = False
        client.loop()

if __name__ == "__main__":
    sys.stdout.flush()
    choose4()
    main()

