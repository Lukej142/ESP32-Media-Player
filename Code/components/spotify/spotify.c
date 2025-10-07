#include "spotify.h"
#include "globals.h"

const char *spotifyApiUrl = "https://api.spotify.com/v1/me/player";
const char *spotifyTokenURL = "https://accounts.spotify.com/api/token";
const char *CONTENT_TYPE = "application/x-www-form-urlencoded";

char local_response_buffer[15360]; // 15KB