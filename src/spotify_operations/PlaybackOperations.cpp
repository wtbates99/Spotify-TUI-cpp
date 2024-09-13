#include "spotify_operations/PlaybackOperations.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>

bool play_music(const std::string &access_token) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/player/play";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool pause_music(const std::string &access_token) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/player/pause";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool skip_track(const std::string &access_token) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/player/next";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool set_volume(const std::string &access_token, int volume) {
  if (volume < 0 || volume > 100)
    return false;
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.spotify.com/v1/me/player/volume?volume_percent=" +
        std::to_string(volume);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool toggle_shuffle(const std::string &access_token, bool enable) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url = "https://api.spotify.com/v1/me/player/shuffle?state=" +
                      std::string(enable ? "true" : "false");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

bool toggle_repeat(const std::string &access_token, const std::string &state) {
  if (state != "track" && state != "context" && state != "off")
    return false;
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.spotify.com/v1/me/player/repeat?state=" + state;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
  }
  return false;
}

void playback_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Playback Control Menu ---\n";
    std::cout << "1. Play\n";
    std::cout << "2. Pause\n";
    std::cout << "3. Skip Track\n";
    std::cout << "4. Set Volume\n";
    std::cout << "5. Toggle Shuffle\n";
    std::cout << "6. Toggle Repeat\n";
    std::cout << "b. Back to Main Menu\n";
    std::cout << "Select an option: ";

    std::string choice = get_input("");

    if (choice == "1") {
      if (play_music(access_token)) {
        std::cout << "Playback started.\n";
      } else {
        std::cout << "Failed to start playback.\n";
      }
    } else if (choice == "2") {
      if (pause_music(access_token)) {
        std::cout << "Playback paused.\n";
      } else {
        std::cout << "Failed to pause playback.\n";
      }
    } else if (choice == "3") {
      if (skip_track(access_token)) {
        std::cout << "Skipped to next track.\n";
      } else {
        std::cout << "Failed to skip track.\n";
      }
    } else if (choice == "4") {
      std::string vol_str = get_input("Enter volume (0-100): ");
      int volume = std::stoi(vol_str);
      if (set_volume(access_token, volume)) {
        std::cout << "Volume set to " << volume << "%.\n";
      } else {
        std::cout << "Failed to set volume.\n";
      }
    } else if (choice == "5") {
      std::string toggle = get_input("Enable shuffle? (y/n): ");
      bool enable = (toggle == "y" || toggle == "Y");
      if (toggle_shuffle(access_token, enable)) {
        std::cout << "Shuffle " << (enable ? "enabled.\n" : "disabled.\n");
      } else {
        std::cout << "Failed to toggle shuffle.\n";
      }
    } else if (choice == "6") {
      std::cout << "Select repeat mode:\n";
      std::cout << "1. Track\n";
      std::cout << "2. Context\n";
      std::cout << "3. Off\n";
      std::cout << "Choice: ";
      std::string rep_choice = get_input("");
      std::string state;
      if (rep_choice == "1")
        state = "track";
      else if (rep_choice == "2")
        state = "context";
      else if (rep_choice == "3")
        state = "off";
      else {
        std::cout << "Invalid choice.\n";
        continue;
      }
      if (toggle_repeat(access_token, state)) {
        std::cout << "Repeat set to " << state << ".\n";
      } else {
        std::cout << "Failed to set repeat.\n";
      }
    } else if (choice == "b" || choice == "B") {
      break;
    } else {
      std::cout << "Invalid option. Try again.\n";
    }
  }
}
