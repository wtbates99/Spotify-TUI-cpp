#ifndef RECOMMENDATIONS_OPERATIONS_H
#define RECOMMENDATIONS_OPERATIONS_H

#include "rapidjson/document.h"
#include <string>
#include <vector>

void recommendations_menu(const std::string &access_token);
bool get_available_genres(const std::string &access_token,
                          rapidjson::Document &available_genres);
std::vector<std::string>
display_available_genres_and_select(const rapidjson::Document &available_genres,
                                    int max_selection = 3);
bool get_recommendations(const std::string &access_token,
                         const std::vector<std::string> &seed_genres,
                         rapidjson::Document &recommendations, int limit = 20);
std::vector<std::pair<std::string, std::string>>
display_recommendations_and_select(const rapidjson::Document &recommendations);
void play_recommended_track(const std::string &access_token,
                            const std::string &track_uri);

#endif // RECOMMENDATIONS_OPERATIONS_H
