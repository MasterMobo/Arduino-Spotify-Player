import speech_recognition
import spotipy
from spotipy.oauth2 import SpotifyOAuth


CLIENT_ID = "55985fc6751c4c5ea97d044ebe0861ca"
CLIENT_SECRET = "e680c5ac23264c0593a3ea15148b172d"
REDIRECT_URI = "https://localhost:8080"

scope = 'user-modify-playback-state, user-read-playback-state'

spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID, client_secret=CLIENT_SECRET, redirect_uri=REDIRECT_URI,scope=scope))

recognizer = speech_recognition.Recognizer()

commandWords = {
    ("start", "continue"): spotify.start_playback,
    ("stop", "pause"):spotify.pause_playback,
    ("skip", "next"):spotify.next_track,
    ("last", "previous", "go back"):spotify.previous_track,
    ("shuffle","randomize"):shuffleCheck,
    ("repeat", "loop"):repeatCheck,
}

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
        

print("Finished Initializing!")


while True:
    try:
        with speech_recognition.Microphone() as mic:
            recognizer.adjust_for_ambient_noise(mic, duration=0.2)
            audio = recognizer.listen(mic)
            
            words = recognizer.recognize_google(audio).lower() 
            
            print("Recognized: ", words)
            
            # Voice Commands
            for key in commandWords.keys():
                if any(command in words for command in key):
                    commandWords[key]()
                    break
    except:
        recognizer = speech_recognition.Recognizer()
        continue
        
        


