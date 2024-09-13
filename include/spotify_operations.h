#ifndef SPOTIFY_OPERATIONS_H
#define SPOTIFY_OPERATIONS_H

#include "rapidjson/document.h"
#include <string>

// Function prototypes for Spotify operations
bool get_user_playlists(const std::string &access_token,
                        rapidjson::Document &playlists);
void display_playlists(const rapidjson::Document &playlists);

#endif // SPOTIFY_OPERATIONS_H
