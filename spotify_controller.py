#--Libraries----------------------------------------------------------------------------------------------------------------------------------------
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import time
import threading
import serial


#--Setup--------------------------------------------------------------------------------------------------------------------------------------------
CLIENT_ID = "55985fc6751c4c5ea97d044ebe0861ca"
CLIENT_SECRET = "e680c5ac23264c0593a3ea15148b172d"
REDIRECT_URI = "https://localhost:8080"

scope = 'user-modify-playback-state, user-read-playback-state'

# Setup Spotify API
spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID, client_secret=CLIENT_SECRET, redirect_uri=REDIRECT_URI,scope=scope))

commands = {
    "pause": spotify.pause_playback,
    "play": spotify.start_playback,
    "prev": spotify.previous_track,
    "next": spotify.next_track,
    "shuffleOn": lambda: spotify.shuffle(True),     # Using lambda because we want to return the function object, not execute it yet
    "shuffleOff": lambda: spotify.shuffle(False),
    "repeattrack": lambda: spotify.repeat("track"),
    "repeatcontext": lambda: spotify.repeat("context"),
    "repeatoff": lambda: spotify.repeat("off")
}


# Function for opening serial port with error handling
def openPort():
    global ser
    try:
        ser = serial.Serial("/dev/ttyACM0",115200,timeout=1)
        ser.reset_input_buffer()
        print("Successfully opened port!")
    except serial.serialutil.SerialException:
        print("Could not open port, retrying in 1 second")
        time.sleep(1)
        openPort()
ser = None
openPort()      


#--Main-Loop---------------------------------------------------------------------------------------------------------------------------------------
while True:
    # Get current song data from API
    current_playback = spotify.current_playback()
    
    if current_playback == None:
        print("Doesn't seem to be playing anything")
        time.sleep(0.5)
        continue
    
    #print(f"Currently playing: " + current_playback["item"]["name"])
    
    # Send song data to Arduino via serial 
    # Message format: songName > artistName @ isPlaying # progress $ duration % repeatState ^ shuffleState |
    try:
        message = current_playback["item"]["name"] + ">" + ', '.join([artist["name"] for artist in current_playback["item"]["album"]["artists"]]) + "@" + ("1" if current_playback["is_playing"] else "0") + "#" + str(current_playback["progress_ms"]) + "$" + str(current_playback["item"]["duration_ms"]) + "%" + current_playback["repeat_state"] + "^" + ("1" if current_playback["shuffle_state"] else "0") + "|"
    except:
        print("Could not process current playback, I'll try again")
        continue

    ser.write(bytes(message, "utf-8"))
    
    
    # Read Arduino message (if any is sent)
    arduino_message = ser.readline().decode("utf-8").strip()
    
    if arduino_message in commands:
        print("Recieved message from arduino:", arduino_message)
    
        # Execute command corresponding to Arduino message
        try:
            commands[arduino_message]()
            continue;
        except:
            print("Couldn't execute the command")
    
        
    time.sleep(0.5)

