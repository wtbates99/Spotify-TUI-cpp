#ifndef SEARCH_OPERATIONS_H
#define SEARCH_OPERATIONS_H

#include "rapidjson/document.h"
#include <string>
#include <vector>

enum class SearchType { TRACK, ARTIST, ALBUM, PLAYLIST };

void search_menu(const std::string &access_token);
bool search_spotify(const std::string &access_token, const std::string &query,
                    SearchType type, rapidjson::Document &results,
                    int limit = 10);
void display_search_results(const rapidjson::Document &results,
                            SearchType type);
std::vector<std::pair<std::string, std::string>>
select_from_search_results(const rapidjson::Document &results, SearchType type);
bool add_track_to_playlist(const std::string &access_token,
                           const std::string &playlist_id,
                           const std::string &track_uri);
bool add_track_to_queue(const std::string &access_token,
                        const std::string &track_uri);

#endif // SEARCH_OPERATIONS_H
