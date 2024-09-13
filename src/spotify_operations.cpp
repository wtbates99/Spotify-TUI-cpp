// spotify_operations.cpp
#include "spotify_operations.h"
#include "rapidjson/error/en.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>
#include <utility>
#include <vector>

// ------------------ Playlist Operations ------------------

bool get_user_playlists(const std::string &access_token,
                        rapidjson::Document &playlists) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL,
                     "https://api.spotify.com/v1/me/playlists");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Parse JSON response
    if (playlists.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(playlists.GetParseError())
                << std::endl;
      return false;
    }

    return true;
  }
  return false;
}

void display_playlists(const rapidjson::Document &playlists) {
  if (!playlists.HasMember("items") || !playlists["items"].IsArray()) {
    std::cerr << "No playlists found." << std::endl;
    return;
  }

  const rapidjson::Value &items = playlists["items"];
  std::vector<std::pair<std::string, std::string>>
      playlist_names; // Pair of name and id

  for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
    const rapidjson::Value &item = items[i];
    if (item.HasMember("name") && item["name"].IsString() &&
        item.HasMember("id") && item["id"].IsString()) {
      playlist_names.emplace_back(item["name"].GetString(),
                                  item["id"].GetString());
    }
  }

  // Display playlists
  std::cout << "\nYour Playlists:\n";
  for (size_t i = 0; i < playlist_names.size(); ++i) {
    std::cout << i + 1 << ". " << playlist_names[i].first << std::endl;
  }

  std::cout
      << "\nEnter the number of the playlist to view details, or 'q' to quit: ";
  std::string input;
  std::getline(std::cin, input);

  if (input == "q" || input == "Q") {
    return;
  }

  try {
    int choice = std::stoi(input);
    if (choice > 0 && choice <= static_cast<int>(playlist_names.size())) {
      std::cout << "You selected: " << playlist_names[choice - 1].first
                << std::endl;
      // Fetch and display playlist details
      rapidjson::Document playlist_details;
      if (get_playlist_details(
              "YOUR_ACCESS_TOKEN", playlist_names[choice - 1].second,
              playlist_details)) { // Replace with actual access token
        display_playlist_details(playlist_details);
      } else {
        std::cerr << "Failed to retrieve playlist details." << std::endl;
      }
    } else {
      std::cout << "Invalid selection." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "Invalid input." << std::endl;
  }
}

bool get_playlist_details(const std::string &access_token,
                          const std::string &playlist_id,
                          rapidjson::Document &playlist_details) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());

    std::string url =
        "https://api.spotify.com/v1/playlists/" + playlist_id +
        "?fields=name,description,tracks.items(track(name,artists(name)))";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Parse JSON response
    if (playlist_details.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(playlist_details.GetParseError())
                << std::endl;
      return false;
    }

    return true;
  }
  return false;
}

void display_playlist_details(const rapidjson::Document &playlist_details) {
  if (!playlist_details.IsObject()) {
    std::cerr << "Invalid playlist details." << std::endl;
    return;
  }

  // Display playlist name and description
  if (playlist_details.HasMember("name") &&
      playlist_details["name"].IsString()) {
    std::cout << "Playlist Name: " << playlist_details["name"].GetString()
              << std::endl;
  }

  if (playlist_details.HasMember("description") &&
      playlist_details["description"].IsString()) {
    std::cout << "Description: " << playlist_details["description"].GetString()
              << std::endl;
  }

  // Display tracks
  if (playlist_details.HasMember("tracks") &&
      playlist_details["tracks"].IsObject()) {
    const rapidjson::Value &tracks = playlist_details["tracks"];
    if (tracks.HasMember("items") && tracks["items"].IsArray()) {
      const rapidjson::Value &items = tracks["items"];
      std::cout << "\nTracks:\n";
      for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
        const rapidjson::Value &item = items[i];
        if (item.HasMember("track") && item["track"].IsObject()) {
          const rapidjson::Value &track = item["track"];
          if (track.HasMember("name") && track["name"].IsString()) {
            std::cout << i + 1 << ". " << track["name"].GetString();
            if (track.HasMember("artists") && track["artists"].IsArray()) {
              std::cout << " by ";
              const rapidjson::Value &artists = track["artists"];
              for (rapidjson::SizeType j = 0; j < artists.Size(); ++j) {
                if (artists[j].HasMember("name") &&
                    artists[j]["name"].IsString()) {
                  std::cout << artists[j]["name"].GetString();
                  if (j != artists.Size() - 1)
                    std::cout << ", ";
                }
              }
            }
            std::cout << std::endl;
          }
        }
      }
    }
  }
}

bool create_playlist(const std::string &access_token,
                     const std::string &user_id,
                     const std::string &playlist_name,
                     rapidjson::Document &new_playlist) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string url =
        "https://api.spotify.com/v1/users/" + user_id + "/playlists";
    std::string json_body =
        "{\"name\":\"" + playlist_name + "\", \"public\":false}";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Parse JSON response
    if (new_playlist.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(new_playlist.GetParseError())
                << std::endl;
      return false;
    }

    return true;
  }
  return false;
}

bool add_tracks_to_playlist(const std::string &access_token,
                            const std::string &playlist_id,
                            const std::vector<std::string> &track_uris) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string url =
        "https://api.spotify.com/v1/playlists/" + playlist_id + "/tracks";

    // Construct JSON body
    std::string json_body = "{\"uris\":[";
    for (size_t i = 0; i < track_uris.size(); ++i) {
      json_body += "\"" + track_uris[i] + "\"";
      if (i != track_uris.size() - 1)
        json_body += ",";
    }
    json_body += "]}";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Optionally, parse and handle the response
    return true;
  }
  return false;
}

// ------------------ Playback Control ------------------

bool get_current_playback(const std::string &access_token,
                          rapidjson::Document &playback) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/me/player";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res == CURLE_OK) {
      if (response.empty()) {
        std::cerr << "No active device found." << std::endl;
        return false;
      }
      if (playback.Parse(response.c_str()).HasParseError()) {
        std::cerr << "JSON parse error: "
                  << rapidjson::GetParseError_En(playback.GetParseError())
                  << std::endl;
        return false;
      }
      return true;
    } else {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
  }
  return false;
}

void display_current_playback(const rapidjson::Document &playback) {
  if (!playback.IsObject()) {
    std::cerr << "Invalid playback information." << std::endl;
    return;
  }

  if (playback.HasMember("item") && playback["item"].IsObject()) {
    const rapidjson::Value &item = playback["item"];
    if (item.HasMember("name") && item["name"].IsString()) {
      std::cout << "Currently Playing: " << item["name"].GetString()
                << std::endl;
    }
    if (item.HasMember("artists") && item["artists"].IsArray()) {
      std::cout << "Artist(s): ";
      const rapidjson::Value &artists = item["artists"];
      for (rapidjson::SizeType i = 0; i < artists.Size(); ++i) {
        if (artists[i].HasMember("name") && artists[i]["name"].IsString()) {
          std::cout << artists[i]["name"].GetString();
          if (i != artists.Size() - 1)
            std::cout << ", ";
        }
      }
      std::cout << std::endl;
    }
  }

  if (playback.HasMember("is_playing") && playback["is_playing"].IsBool()) {
    std::cout << "Playback Status: "
              << (playback["is_playing"].GetBool() ? "Playing" : "Paused")
              << std::endl;
  }
}

bool play_music(const std::string &access_token, const std::string &device_id) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/me/player/play";
    if (!device_id.empty()) {
      url += "?device_id=" + url_encode(device_id);
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);        // Use PUT for this endpoint
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); // Empty body
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res == CURLE_OK) {
      // A 204 No Content response indicates success
      std::cout << "Playback started." << std::endl;
      return true;
    } else {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
  }
  return false;
}

bool pause_music(const std::string &access_token,
                 const std::string &device_id) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/me/player/pause";
    if (!device_id.empty()) {
      url += "?device_id=" + url_encode(device_id);
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);        // Use PUT for this endpoint
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); // Empty body
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res == CURLE_OK) {
      // A 204 No Content response indicates success
      std::cout << "Playback paused." << std::endl;
      return true;
    } else {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
  }
  return false;
}

bool next_track(const std::string &access_token, const std::string &device_id) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/me/player/next";
    if (!device_id.empty()) {
      url += "?device_id=" + url_encode(device_id);
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L); // Use POST for this endpoint
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); // Empty body
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res == CURLE_OK) {
      // A 204 No Content response indicates success
      std::cout << "Skipped to the next track." << std::endl;
      return true;
    } else {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
  }
  return false;
}

// ------------------ Recommendations ------------------

bool get_recommendations(const std::string &access_token,
                         const std::vector<std::string> &seed_artists,
                         const std::vector<std::string> &seed_tracks,
                         const std::vector<std::string> &seed_genres,
                         rapidjson::Document &recommendations, int limit) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/recommendations?limit=" +
                      std::to_string(limit);

    // Append seed parameters
    if (!seed_artists.empty()) {
      url += "&seed_artists=";
      for (size_t i = 0; i < seed_artists.size(); ++i) {
        url += seed_artists[i];
        if (i != seed_artists.size() - 1)
          url += ",";
      }
    }

    if (!seed_tracks.empty()) {
      url += "&seed_tracks=";
      for (size_t i = 0; i < seed_tracks.size(); ++i) {
        url += seed_tracks[i];
        if (i != seed_tracks.size() - 1)
          url += ",";
      }
    }

    if (!seed_genres.empty()) {
      url += "&seed_genres=";
      for (size_t i = 0; i < seed_genres.size(); ++i) {
        url += seed_genres[i];
        if (i != seed_genres.size() - 1)
          url += ",";
      }
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Parse JSON response
    if (recommendations.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(recommendations.GetParseError())
                << std::endl;
      return false;
    }

    return true;
  }
  return false;
}

void display_recommendations(const rapidjson::Document &recommendations) {
  if (!recommendations.HasMember("tracks") ||
      !recommendations["tracks"].IsArray()) {
    std::cerr << "No recommendations found." << std::endl;
    return;
  }

  const rapidjson::Value &tracks = recommendations["tracks"];
  std::cout << "\nRecommended Tracks:\n";
  for (rapidjson::SizeType i = 0; i < tracks.Size(); ++i) {
    const rapidjson::Value &track = tracks[i];
    if (track.HasMember("name") && track["name"].IsString()) {
      std::cout << i + 1 << ". " << track["name"].GetString();
      if (track.HasMember("artists") && track["artists"].IsArray()) {
        std::cout << " by ";
        const rapidjson::Value &artists = track["artists"];
        for (rapidjson::SizeType j = 0; j < artists.Size(); ++j) {
          if (artists[j].HasMember("name") && artists[j]["name"].IsString()) {
            std::cout << artists[j]["name"].GetString();
            if (j != artists.Size() - 1)
              std::cout << ", ";
          }
        }
      }
      std::cout << std::endl;
    }
  }
}

// ------------------ Search ------------------

bool search_spotify(const std::string &access_token, const std::string &query,
                    const std::string &type,
                    rapidjson::Document &search_results, int limit) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    // URL encode the query
    CURLcode res_code;
    char *encoded_query = curl_easy_escape(curl, query.c_str(), query.length());
    if (!encoded_query) {
      std::cerr << "Failed to URL encode the query." << std::endl;
      curl_easy_cleanup(curl);
      return false;
    }

    std::string url =
        "https://api.spotify.com/v1/search?q=" + std::string(encoded_query) +
        "&type=" + type + "&limit=" + std::to_string(limit);
    curl_free(encoded_query);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    res_code = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res_code != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res_code) << std::endl;
      return false;
    }

    // Parse JSON response
    if (search_results.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(search_results.GetParseError())
                << std::endl;
      return false;
    }

    return true;
  }
  return false;
}

void display_search_results(const rapidjson::Document &search_results,
                            const std::string &type) {
  // Use c_str() for HasMember and [] operator
  if (!search_results.HasMember(type.c_str()) ||
      !search_results[type.c_str()].HasMember("items")) {
    std::cerr << "No search results found." << std::endl;
    return;
  }

  const rapidjson::Value &items = search_results[type.c_str()]["items"];
  std::cout << "\nSearch Results (" << type << "):\n";
  for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
    const rapidjson::Value &item = items[i];
    if (item.HasMember("name") && item["name"].IsString()) {
      std::cout << i + 1 << ". " << item["name"].GetString() << std::endl;
    }
  }
}

// ------------------ User Information ------------------

bool get_user_profile(const std::string &access_token,
                      rapidjson::Document &user_profile) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/me";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res == CURLE_OK) {
      if (response.empty()) {
        std::cerr << "No user profile found." << std::endl;
        return false;
      }
      if (user_profile.Parse(response.c_str()).HasParseError()) {
        std::cerr << "JSON parse error: "
                  << rapidjson::GetParseError_En(user_profile.GetParseError())
                  << std::endl;
        return false;
      }
      return true;
    } else {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
  }
  return false;
}

void display_user_profile(const rapidjson::Document &user_profile) {
  if (!user_profile.IsObject()) {
    std::cerr << "Invalid user profile." << std::endl;
    return;
  }

  if (user_profile.HasMember("display_name") &&
      user_profile["display_name"].IsString()) {
    std::cout << "User: " << user_profile["display_name"].GetString()
              << std::endl;
  }

  if (user_profile.HasMember("email") && user_profile["email"].IsString()) {
    std::cout << "Email: " << user_profile["email"].GetString() << std::endl;
  }

  if (user_profile.HasMember("id") && user_profile["id"].IsString()) {
    std::cout << "User ID: " << user_profile["id"].GetString() << std::endl;
  }
}

// ------------------ Library Management ------------------

bool get_saved_tracks(const std::string &access_token,
                      rapidjson::Document &saved_tracks, int limit) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url =
        "https://api.spotify.com/v1/me/tracks?limit=" + std::to_string(limit);
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Parse JSON response
    if (saved_tracks.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(saved_tracks.GetParseError())
                << std::endl;
      return false;
    }

    return true;
  }
  return false;
}

void display_saved_tracks(const rapidjson::Document &saved_tracks) {
  if (!saved_tracks.HasMember("items") || !saved_tracks["items"].IsArray()) {
    std::cerr << "No saved tracks found." << std::endl;
    return;
  }

  const rapidjson::Value &items = saved_tracks["items"];
  std::cout << "\nYour Saved Tracks:\n";
  for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
    const rapidjson::Value &item = items[i];
    if (item.HasMember("track") && item["track"].IsObject()) {
      const rapidjson::Value &track = item["track"];
      if (track.HasMember("name") && track["name"].IsString()) {
        std::cout << i + 1 << ". " << track["name"].GetString();
        if (track.HasMember("artists") && track["artists"].IsArray()) {
          std::cout << " by ";
          const rapidjson::Value &artists = track["artists"];
          for (rapidjson::SizeType j = 0; j < artists.Size(); ++j) {
            if (artists[j].HasMember("name") && artists[j]["name"].IsString()) {
              std::cout << artists[j]["name"].GetString();
              if (j != artists.Size() - 1)
                std::cout << ", ";
            }
          }
        }
        std::cout << std::endl;
      }
    }
  }
}

bool save_track(const std::string &access_token, const std::string &track_id) {
  CURL *curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    std::string url = "https://api.spotify.com/v1/me/tracks?ids=" + track_id;

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);        // Use PUT to save a track
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); // Empty body
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res == CURLE_OK) {
      // A 200 OK or 204 No Content response indicates success
      std::cout << "Track saved to your library." << std::endl;
      return true;
    } else {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
  }
  return false;
}

// ------------------ Additional Operations ------------------

// Add any additional function implementations here as needed for other Spotify
// API operations.
