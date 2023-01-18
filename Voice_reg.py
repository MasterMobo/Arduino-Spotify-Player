#--Libraries----------------------------------------------------------------------------------------------------------------------------------------
import speech_recognition
import sounddevice
import spotipy
from spotipy.oauth2 import SpotifyOAuth

#--Setup--------------------------------------------------------------------------------------------------------------------------------------------

# Functions for handling voice commands
def shuffleCheck():
    if "on" in words:
        spotify.shuffle(True)
    elif "off" in words:
        spotify.shuffle(False)

def repeatCheck():
    if "off" in words or "stop" in words:
        spotify.repeat("off")
    elif "on" in words or "track" in words or "song" in words:
        spotify.repeat("track")
    elif "album" in words or "playlist" in words:
        spotify.repeat("context")

CLIENT_ID = "55985fc6751c4c5ea97d044ebe0861ca"
CLIENT_SECRET = "e680c5ac23264c0593a3ea15148b172d"
REDIRECT_URI = "https://localhost:8080"

scope = 'user-modify-playback-state, user-read-playback-state'

# Setup Spotify API
spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID, client_secret=CLIENT_SECRET, redirect_uri=REDIRECT_URI,scope=scope))

# Setup speech recoginzer
recognizer = speech_recognition.Recognizer()

# Dictionary of words for voice commands
commandWords = {
    ("start", "continue"): spotify.start_playback,
    ("stop", "pause"):spotify.pause_playback,
    ("skip", "next"):spotify.next_track,
    ("last", "previous", "go back"):spotify.previous_track,
    ("shuffle","randomize"):shuffleCheck,
    ("repeat", "loop"):repeatCheck,
}

        

print("Finished Initializing!")

#--Main-Loop---------------------------------------------------------------------------------------------------------------------------------------
while True:
    print("=======================================================================")
    try:
        print("Waiting for command")
        with speech_recognition.Microphone() as mic:                # Setup microphone
            recognizer.adjust_for_ambient_noise(mic, duration=0.2)
            audio = recognizer.listen(mic)
            
        words = recognizer.recognize_google(audio).lower()      # Get speech recogition result
        
        print("Recognized: ", words)
        
        # Voice Commands
        for key in commandWords.keys():                         # Loop through command dictionary
            if any(command in words for command in key):        # If find any command in speech recogition result, execute that command
                try:                                            
                    commandWords[key]() 
                except:
                    print("Couldn't execute command")
                break
                    
    except speech_recognition.UnknownValueError:     # If encounter error, re-initialize the speech recognizer
        print("Sorry, I couln't understand you")
        recognizer = speech_recognition.Recognizer()
        continue
        
        


