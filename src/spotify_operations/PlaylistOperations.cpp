#include "spotify_operations/PlaylistOperations.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>

// Callback function to handle cURL responses for playlists
size_t WriteCallbackPlaylists(void *contents, size_t size, size_t nmemb,
                              void *userp) {
  size_t totalSize = size * nmemb;
  std::string *playlists = static_cast<std::string *>(userp);
  playlists->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

// Fetches the user's playlists from Spotify
bool get_user_playlists(const std::string &access_token,
                        rapidjson::Document &playlists, int limit, int offset) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/playlists?limit=" +
                      std::to_string(limit) +
                      "&offset=" + std::to_string(offset);
    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackPlaylists);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
      return false;

    if (playlists.Parse(response.c_str()).HasParseError())
      return false;
    return true;
  }
  return false;
}

// Displays playlists and allows user to select one
std::vector<std::pair<std::string, std::string>>
display_playlists_and_select(const rapidjson::Document &playlists) {
  std::vector<std::pair<std::string, std::string>> selected_playlists;
  if (playlists.HasMember("items") && playlists["items"].IsArray()) {
    const rapidjson::Value &items = playlists["items"];
    std::cout << "\nYour Playlists:\n";
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string id = item["id"].GetString();
      std::cout << "- " << name << " (ID: " << id << ")\n";
      selected_playlists.emplace_back(name, id);
    }
  }
  return selected_playlists;
}

// Callback function to handle cURL responses for tracks
size_t WriteCallbackTracks(void *contents, size_t size, size_t nmemb,
                           void *userp) {
  size_t totalSize = size * nmemb;
  std::string *tracks = static_cast<std::string *>(userp);
  tracks->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

// Fetches tracks from a specific playlist
bool get_playlist_tracks(const std::string &access_token,
                         const std::string &playlist_id,
                         rapidjson::Document &tracks, int limit, int offset) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/playlists/" + playlist_id +
                      "/tracks?limit=" + std::to_string(limit) +
                      "&offset=" + std::to_string(offset);
    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackTracks);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
      return false;

    if (tracks.Parse(response.c_str()).HasParseError())
      return false;
    return true;
  }
  return false;
}

// Displays tracks and allows user to select one
std::vector<std::pair<std::string, std::string>>
display_tracks_and_select(const rapidjson::Document &tracks) {
  std::vector<std::pair<std::string, std::string>> selected_tracks;
  if (tracks.HasMember("items") && tracks["items"].IsArray()) {
    const rapidjson::Value &items = tracks["items"];
    std::cout << "\nTracks in Playlist:\n";
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

// Sends a request to play a selected track
void play_selected_track(const std::string &access_token,
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

// Main playlist menu function
void playlist_menu(const std::string &access_token) {
  rapidjson::Document playlists_doc;
  if (get_user_playlists(access_token, playlists_doc)) {
    auto playlists = display_playlists_and_select(playlists_doc);
    if (playlists.empty()) {
      std::cout << "No playlists found.\n";
      return;
    }
    std::string choice =
        get_input("\nEnter the name of the playlist to view tracks: ");
    std::string selected_id;
    for (auto &pl : playlists) {
      if (pl.first == choice) {
        selected_id = pl.second;
        break;
      }
    }
    if (selected_id.empty()) {
      std::cout << "Playlist not found.\n";
      return;
    }
    rapidjson::Document tracks_doc;
    if (get_playlist_tracks(access_token, selected_id, tracks_doc)) {
      auto tracks = display_tracks_and_select(tracks_doc);
      if (tracks.empty()) {
        std::cout << "No tracks found in this playlist.\n";
        return;
      }
      std::string track_choice =
          get_input("\nEnter the name of the track to play: ");
      std::string selected_uri;
      for (auto &tr : tracks) {
        if (tr.first == track_choice) {
          selected_uri = tr.second;
          break;
        }
      }
      if (selected_uri.empty()) {
        std::cout << "Track not found.\n";
        return;
      }
      play_selected_track(access_token, selected_uri);
    } else {
      std::cout << "Failed to retrieve tracks.\n";
    }
  } else {
    std::cout << "Failed to retrieve playlists.\n";
  }
}
