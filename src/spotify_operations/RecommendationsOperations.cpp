#include "spotify_operations/RecommendationsOperations.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>

size_t WriteCallbackGenres(void *contents, size_t size, size_t nmemb,
                           void *userp) {
  size_t totalSize = size * nmemb;
  std::string *genres = static_cast<std::string *>(userp);
  genres->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

bool get_available_genres(const std::string &access_token,
                          rapidjson::Document &available_genres) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.spotify.com/v1/recommendations/available-genre-seeds";
    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackGenres);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
      return false;

    if (available_genres.Parse(response.c_str()).HasParseError())
      return false;
    return true;
  }
  return false;
}

std::vector<std::string>
display_available_genres_and_select(const rapidjson::Document &available_genres,
                                    int max_selection) {
  std::vector<std::string> selected_genres;
  if (available_genres.HasMember("genres") &&
      available_genres["genres"].IsArray()) {
    const rapidjson::Value &genres = available_genres["genres"];
    std::cout << "Available Genres:\n";
    for (auto &genre : genres.GetArray()) {
      std::cout << "- " << genre.GetString() << "\n";
    }
    std::cout << "\nEnter up to " << max_selection
              << " genres separated by commas: ";
    std::string input = get_input("");
    auto tokens = split(input, ',');
    for (auto &token : tokens) {
      std::string genre = trim(token);
      if (!genre.empty())
        selected_genres.push_back(genre);
      if (selected_genres.size() == static_cast<size_t>(max_selection))
        break;
    }
  }
  return selected_genres;
}

size_t WriteCallbackRecommendations(void *contents, size_t size, size_t nmemb,
                                    void *userp) {
  size_t totalSize = size * nmemb;
  std::string *recommendations = static_cast<std::string *>(userp);
  recommendations->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

bool get_recommendations(const std::string &access_token,
                         const std::vector<std::string> &seed_genres,
                         rapidjson::Document &recommendations, int limit) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/recommendations?limit=" +
                      std::to_string(limit);
    for (auto &genre : seed_genres) {
      url += "&seed_genres=" + url_encode(genre);
    }
    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackRecommendations);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
      return false;

    if (recommendations.Parse(response.c_str()).HasParseError())
      return false;
    return true;
  }
  return false;
}

std::vector<std::pair<std::string, std::string>>
display_recommendations_and_select(const rapidjson::Document &recommendations) {
  std::vector<std::pair<std::string, std::string>> selected_tracks;
  if (recommendations.HasMember("tracks") &&
      recommendations["tracks"].IsArray()) {
    const rapidjson::Value &tracks = recommendations["tracks"];
    for (auto &track : tracks.GetArray()) {
      std::string name = track["name"].GetString();
      std::string uri = track["uri"].GetString();
      std::cout << "- " << name << " (URI: " << uri << ")\n";
      selected_tracks.emplace_back(name, uri);
    }
    std::cout << "\nEnter the name of the track to play: ";
    std::string choice = get_input("");
    for (auto &tr : selected_tracks) {
      if (tr.first == choice) {
        return {tr};
      }
    }
    std::cout << "Track not found.\n";
  }
  return selected_tracks;
}

void play_recommended_track(const std::string &access_token,
                            const std::string &track_uri) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/player/play";
    std::string json_body = "{ \"uris\": [\"" + track_uri + "\"] }";

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      std::cout << "Track is now playing.\n";
    } else {
      std::cerr << "Failed to play track: " << curl_easy_strerror(res) << "\n";
    }
  }
}

void recommendations_menu(const std::string &access_token) {
  rapidjson::Document genres_doc;
  if (get_available_genres(access_token, genres_doc)) {
    auto selected_genres = display_available_genres_and_select(genres_doc);
    if (selected_genres.empty()) {
      std::cout << "No genres selected.\n";
      return;
    }
    rapidjson::Document recommendations_doc;
    if (get_recommendations(access_token, selected_genres,
                            recommendations_doc)) {
      auto recommended_tracks =
          display_recommendations_and_select(recommendations_doc);
      if (recommended_tracks.empty()) {
        std::cout << "No recommendations found.\n";
        return;
      }
      play_recommended_track(access_token, recommended_tracks[0].second);
    } else {
      std::cout << "Failed to get recommendations.\n";
    }
  } else {
    std::cout << "Failed to retrieve available genres.\n";
  }
}
