#ifndef PLAYLIST_OPERATIONS_H
#define PLAYLIST_OPERATIONS_H

#include "rapidjson/document.h"
#include <string>
#include <vector>

void playlist_menu(const std::string &access_token);
bool get_user_playlists(const std::string &access_token,
                        rapidjson::Document &playlists, int limit = 20,
                        int offset = 0);
std::vector<std::pair<std::string, std::string>>
display_playlists_and_select(const rapidjson::Document &playlists);
bool get_playlist_tracks(const std::string &access_token,
                         const std::string &playlist_id,
                         rapidjson::Document &tracks, int limit = 20,
                         int offset = 0);
std::vector<std::pair<std::string, std::string>>
display_tracks_and_select(const rapidjson::Document &tracks);
void play_selected_track(const std::string &access_token,
                         const std::string &track_uri);

#endif // PLAYLIST_OPERATIONS_H
