import spotipy
import spotipy.util as util
from pprint import pprint
import math
from time import sleep
import threading

import serial

ser = serial.Serial(port='COM7', baudrate=115200, timeout=1)

def convertMilis(ms):
    return f"{math.floor((ms / 1000) / 60)}:{math.floor((ms / 1000) % 60)}"


def main():
    threading.Timer(1.0, main).start()
    scope = 'user-modify-playback-state, user-read-playback-state'

    token = util.prompt_for_user_token(scope=scope)

    spotify = spotipy.Spotify(auth=token)
    current_playback = spotify.current_playback()

    # spotify.pause_playback()
    # sleep(5)

    # spotify.previous_track()
    # sleep(5)
    # spotify.seek_track(50000)
    # sleep(5)
    # spotify.next_track()
    # sleep(5)
    
    # print("-------------------------------------------")
    # print("Is playing:", current_playback["is_playing"])
    # print("Song Name:", current_playback["item"]["name"])
    # print("Artist(s):",  ', '.join([artist["name"] for artist in current_playback["item"]["album"]["artists"]]))
    # print("Progress:", convertMilis(current_playback["progress_ms"]))
    # print("Duration:", convertMilis(current_playback["item"]["duration_ms"]))
    # print("Repeat State:", current_playback["repeat_state"])
    # print("Shuffle State:", current_playback["shuffle_state"])
    # # pprint(current_playback)
    # print("-------------------------------------------")
    
    # Message format: songName > artistName @ isPlaying # progress $ duration % repeatState ^ shuffleState &
    message = current_playback["item"]["name"] + ">" + ', '.join([artist["name"] for artist in current_playback["item"]["album"]["artists"]]) + "@" + ("1" if current_playback["is_playing"] else "0") + "#" + str(current_playback["progress_ms"]) + "$" + str(current_playback["item"]["duration_ms"]) + "%" + ("0" if current_playback["repeat_state"] == "off" else "1") + "^" + ("1" if current_playback["shuffle_state"] else "0") + "&"
    # print(message)
    ser.write(bytes(message, "utf-8"))
    sleep(0.5)
    
    arduino_message = ser.readline().decode("utf-8").strip()
    print(arduino_message)
    
    if arduino_message == "pause":
        spotify.pause_playback()
    if arduino_message == "play":
        spotify.start_playback()

main()