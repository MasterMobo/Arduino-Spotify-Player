import speech_recognition
import spotipy
from spotipy.oauth2 import SpotifyOAuth

CLIENT_ID = "55985fc6751c4c5ea97d044ebe0861ca"
CLIENT_SECRET = "e680c5ac23264c0593a3ea15148b172d"
REDIRECT_URI = "https://localhost:8080"

scope = 'user-modify-playback-state, user-read-playback-state'

spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID, client_secret=CLIENT_SECRET, redirect_uri=REDIRECT_URI,scope=scope))

recognizer = speech_recognition.Recognizer()


print("Finished Initializing!")

while True:
    try:
        with speech_recognition.Microphone() as mic:
            recognizer.adjust_for_ambient_noise(mic, duration=0.2)
            audio = recognizer.listen(mic)
            
            words = recognizer.recognize_google(audio).lower()
            
            # Voice Commands
            print("Recognized: ", words)
            if "start" in words or "continue" in words:
                spotify.start_playback()
            elif "stop" in words or "pause" in words:
                spotify.pause_playback()
            elif "skip" in words or "next" in words:
                spotify.next_track()
            elif "last" in words or "previous" in words or "go back" in words:
                spotify.previous_track()
            elif "shuffle" in words and "on" in words:
                spotify.shuffle(True)
            elif "shuffle" in words and "off" in words:
                spotify.shuffle(False)
            elif "repeat" in words or "loop" in words:     # There are repeat 3 modes: off, track (repeat a single song), context (repeat a playlist/album) 
                if "off" in words or "stop" in words:
                    spotify.repeat("off")
                elif "on" in words or "track" in words or "song" in words:
                    spotify.repeat("track")
                elif "album" in words or "playlist" in words:
                    spotify.repeat("context")
            
    except:
        recognizer = speech_recognition.Recognizer()
        continue
        
        


