#include "spotify_operations/LibraryOperations.h"
#include "spotify_operations/SearchOperations.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>

size_t WriteCallbackSavedTracks(void *contents, size_t size, size_t nmemb,
                                void *userp) {
  size_t totalSize = size * nmemb;
  std::string *saved_tracks = static_cast<std::string *>(userp);
  saved_tracks->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

bool get_saved_tracks(const std::string &access_token,
                      rapidjson::Document &saved_tracks, int limit,
                      int offset) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.spotify.com/v1/me/tracks?limit=" + std::to_string(limit) +
        "&offset=" + std::to_string(offset);
    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackSavedTracks);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
      return false;

    if (saved_tracks.Parse(response.c_str()).HasParseError())
      return false;
    return true;
  }
  return false;
}

std::vector<std::pair<std::string, std::string>>
display_saved_tracks_and_select(const rapidjson::Document &saved_tracks) {
  std::vector<std::pair<std::string, std::string>> selected_tracks;
  if (saved_tracks.HasMember("items") && saved_tracks["items"].IsArray()) {
    const rapidjson::Value &items = saved_tracks["items"];
    for (auto &item : items.GetArray()) {
      if (item.HasMember("track") && item["track"].IsObject()) {
        std::string name = item["track"]["name"].GetString();
        std::string uri = item["track"]["uri"].GetString();
        std::cout << "- " << name << " (URI: " << uri << ")\n";
        selected_tracks.emplace_back(name, uri);
      }
    }
  }
  return selected_tracks;
}

bool add_track_to_library(const std::string &access_token,
                          const std::string &track_uri) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.spotify.com/v1/me/tracks?ids=" + url_encode(track_uri);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool remove_track_from_library(const std::string &access_token,
                               const std::string &track_uri) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.spotify.com/v1/me/tracks?ids=" + url_encode(track_uri);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

void library_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Library Management Menu ---\n";
    std::cout << "1. View Saved Tracks\n";
    std::cout << "2. Add a Track to Library\n";
    std::cout << "3. Remove a Track from Library\n";
    std::cout << "b. Back to Main Menu\n";
    std::cout << "Select an option: ";

    std::string choice = get_input("");

    if (choice == "1") {
      rapidjson::Document saved_tracks_doc;
      if (get_saved_tracks(access_token, saved_tracks_doc)) {
        auto tracks = display_saved_tracks_and_select(saved_tracks_doc);
        if (tracks.empty()) {
          std::cout << "No saved tracks found.\n";
          continue;
        }
        std::cout << "Saved Tracks:\n";
        for (auto &tr : tracks) {
          std::cout << "- " << tr.first << " (URI: " << tr.second << ")\n";
        }
      } else {
        std::cout << "Failed to retrieve saved tracks.\n";
      }
    } else if (choice == "2") {
      std::cout << "\n--- Add a Track to Your Library ---\n";
      std::string query = get_input("Enter the name of the track to add: ");
      rapidjson::Document search_results;
      if (search_spotify(access_token, query, SearchType::TRACK,
                         search_results)) {
        display_search_results(search_results, SearchType::TRACK);
        auto selected =
            select_from_search_results(search_results, SearchType::TRACK);
        if (selected.empty()) {
          std::cout << "No track selected.\n";
          continue;
        }
        if (add_track_to_library(access_token, selected[0].second)) {
          std::cout << "Track added to library.\n";
        } else {
          std::cout << "Failed to add track to library.\n";
        }
      } else {
        std::cout << "Search failed.\n";
      }
    } else if (choice == "3") {
      std::cout << "\n--- Remove a Track from Your Library ---\n";
      std::string query = get_input("Enter the name of the track to remove: ");
      rapidjson::Document search_results;
      if (search_spotify(access_token, query, SearchType::TRACK,
                         search_results)) {
        display_search_results(search_results, SearchType::TRACK);
        auto selected =
            select_from_search_results(search_results, SearchType::TRACK);
        if (selected.empty()) {
          std::cout << "No track selected.\n";
          continue;
        }
        if (remove_track_from_library(access_token, selected[0].second)) {
          std::cout << "Track removed from library.\n";
        } else {
          std::cout << "Failed to remove track from library.\n";
        }
      } else {
        std::cout << "Search failed.\n";
      }
    } else if (choice == "b" || choice == "B") {
      break;
    } else {
      std::cout << "Invalid option. Try again.\n";
    }
  }
}
