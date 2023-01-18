def getSong():  # This function will slice the initial commands to get Artist - Song (Some songs have identical names)
    song_artist = words[5:]  # Get both Song and artist
    song = song_artist.split(" ")[0]
    artist = song_artist.split(" ")[-1]
    track_id = spotify.search(q="artist:" + artist + " track:" + song, type="track")
    song_link = "https://api.spotify.com/v1/tracks/{id}".format(id=track_id)
    spotify.start_playback(uris=song_link)
