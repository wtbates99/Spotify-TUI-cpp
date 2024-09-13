#include "spotify_operations/SearchOperations.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>

size_t WriteCallbackSearch(void *contents, size_t size, size_t nmemb,
                           void *userp) {
  size_t totalSize = size * nmemb;
  std::string *results = static_cast<std::string *>(userp);
  results->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

bool search_spotify(const std::string &access_token, const std::string &query,
                    SearchType type, rapidjson::Document &results, int limit) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string type_str;
    switch (type) {
    case SearchType::TRACK:
      type_str = "track";
      break;
    case SearchType::ARTIST:
      type_str = "artist";
      break;
    case SearchType::ALBUM:
      type_str = "album";
      break;
    case SearchType::PLAYLIST:
      type_str = "playlist";
      break;
    default:
      type_str = "track";
      break;
    }
    std::string url =
        "https://api.spotify.com/v1/search?q=" + url_encode(query) +
        "&type=" + type_str + "&limit=" + std::to_string(limit);
    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackSearch);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
      return false;

    if (results.Parse(response.c_str()).HasParseError())
      return false;
    return true;
  }
  return false;
}

void display_search_results(const rapidjson::Document &results,
                            SearchType type) {
  std::cout << "\nSearch Results:\n";
  if (type == SearchType::TRACK && results.HasMember("tracks") &&
      results["tracks"].HasMember("items")) {
    const rapidjson::Value &items = results["tracks"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      std::cout << "- " << name << " (URI: " << uri << ")\n";
    }
  } else if (type == SearchType::ARTIST && results.HasMember("artists") &&
             results["artists"].HasMember("items")) {
    const rapidjson::Value &items = results["artists"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      std::cout << "- " << name << " (URI: " << uri << ")\n";
    }
  } else if (type == SearchType::ALBUM && results.HasMember("albums") &&
             results["albums"].HasMember("items")) {
    const rapidjson::Value &items = results["albums"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      std::cout << "- " << name << " (URI: " << uri << ")\n";
    }
  } else if (type == SearchType::PLAYLIST && results.HasMember("playlists") &&
             results["playlists"].HasMember("items")) {
    const rapidjson::Value &items = results["playlists"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      std::cout << "- " << name << " (URI: " << uri << ")\n";
    }
  }
}

std::vector<std::pair<std::string, std::string>>
select_from_search_results(const rapidjson::Document &results,
                           SearchType type) {
  std::vector<std::pair<std::string, std::string>> selected;
  std::string choice = get_input("Enter the name of the item to select: ");
  if (type == SearchType::TRACK && results.HasMember("tracks") &&
      results["tracks"].HasMember("items")) {
    const rapidjson::Value &items = results["tracks"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      if (name == choice) {
        selected.emplace_back(name, uri);
        break;
      }
    }
  } else if (type == SearchType::ARTIST && results.HasMember("artists") &&
             results["artists"].HasMember("items")) {
    const rapidjson::Value &items = results["artists"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      if (name == choice) {
        selected.emplace_back(name, uri);
        break;
      }
    }
  } else if (type == SearchType::ALBUM && results.HasMember("albums") &&
             results["albums"].HasMember("items")) {
    const rapidjson::Value &items = results["albums"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      if (name == choice) {
        selected.emplace_back(name, uri);
        break;
      }
    }
  } else if (type == SearchType::PLAYLIST && results.HasMember("playlists") &&
             results["playlists"].HasMember("items")) {
    const rapidjson::Value &items = results["playlists"]["items"];
    for (auto &item : items.GetArray()) {
      std::string name = item["name"].GetString();
      std::string uri = item["uri"].GetString();
      if (name == choice) {
        selected.emplace_back(name, uri);
        break;
      }
    }
  }
  return selected;
}

bool add_track_to_playlist(const std::string &access_token,
                           const std::string &playlist_id,
                           const std::string &track_uri) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/playlists/" + playlist_id +
                      "/tracks?uris=" + url_encode(track_uri);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool add_track_to_queue(const std::string &access_token,
                        const std::string &track_uri) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/player/queue?uri=" +
                      url_encode(track_uri);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

void search_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Search Menu ---\n";
    std::cout << "1. Search Tracks\n";
    std::cout << "2. Search Artists\n";
    std::cout << "3. Search Albums\n";
    std::cout << "4. Search Playlists\n";
    std::cout << "b. Back to Main Menu\n";
    std::cout << "Select an option: ";

    std::string choice = get_input("");
    SearchType type;
    if (choice == "1")
      type = SearchType::TRACK;
    else if (choice == "2")
      type = SearchType::ARTIST;
    else if (choice == "3")
      type = SearchType::ALBUM;
    else if (choice == "4")
      type = SearchType::PLAYLIST;
    else if (choice == "b" || choice == "B")
      break;
    else {
      std::cout << "Invalid option. Try again.\n";
      continue;
    }

    std::string query = get_input("Enter search query: ");
    rapidjson::Document results_doc;
    if (search_spotify(access_token, query, type, results_doc)) {
      display_search_results(results_doc, type);
      auto selected = select_from_search_results(results_doc, type);
      if (selected.empty()) {
        std::cout << "No selection made.\n";
        continue;
      }
      if (type == SearchType::TRACK) {
        std::cout << "1. Add to Playlist\n2. Add to Queue\nb. Back\nSelect an "
                     "option: ";
        std::string add_choice = get_input("");
        if (add_choice == "1") {
          std::string playlist_id =
              get_input("Enter Playlist ID to add the track: ");
          if (add_track_to_playlist(access_token, playlist_id,
                                    selected[0].second)) {
            std::cout << "Track added to playlist.\n";
          } else {
            std::cout << "Failed to add track to playlist.\n";
          }
        } else if (add_choice == "2") {
          if (add_track_to_queue(access_token, selected[0].second)) {
            std::cout << "Track added to queue.\n";
          } else {
            std::cout << "Failed to add track to queue.\n";
          }
        } else if (add_choice == "b" || add_choice == "B") {
          continue;
        } else {
          std::cout << "Invalid option.\n";
        }
      }
      // Implement similar options for other search types if needed
    } else {
      std::cout << "Search failed.\n";
    }
  }
}
