#ifndef LIBRARY_OPERATIONS_H
#define LIBRARY_OPERATIONS_H

#include "rapidjson/document.h"
#include <string>
#include <vector>

void library_menu(const std::string &access_token);
bool get_saved_tracks(const std::string &access_token,
                      rapidjson::Document &saved_tracks, int limit = 20,
                      int offset = 0);
std::vector<std::pair<std::string, std::string>>
display_saved_tracks_and_select(const rapidjson::Document &saved_tracks);
bool add_track_to_library(const std::string &access_token,
                          const std::string &track_uri);
bool remove_track_from_library(const std::string &access_token,
                               const std::string &track_uri);

#endif // LIBRARY_OPERATIONS_H
