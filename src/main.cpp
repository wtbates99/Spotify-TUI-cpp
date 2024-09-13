#include "rapidjson/document.h"
#include "spotify_auth.h"
#include "spotify_operations.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>

// Function declarations for submenus
void playlist_menu(const std::string &access_token);
void playback_menu(const std::string &access_token);
void recommendations_menu(const std::string &access_token);
void search_menu(const std::string &access_token);
void user_profile_menu(const std::string &access_token);
void library_menu(const std::string &access_token);

int main() {
  std::string access_token;

  // Authenticate the user and obtain the access token
  if (!authenticate(access_token)) {
    std::cerr << "Authentication failed." << std::endl;
    return 1;
  }

  // Main application loop
  while (true) {
    std::cout << "\n=== Spotify TUI Main Menu ===\n";
    std::cout << "1. Playlists\n";
    std::cout << "2. Playback Control\n";
    std::cout << "3. Recommendations\n";
    std::cout << "4. Search\n";
    std::cout << "5. User Profile\n";
    std::cout << "6. Library Management\n";
    std::cout << "q. Quit\n";
    std::cout << "Select an option: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
      playlist_menu(access_token);
    } else if (choice == "2") {
      playback_menu(access_token);
    } else if (choice == "3") {
      recommendations_menu(access_token);
    } else if (choice == "4") {
      search_menu(access_token);
    } else if (choice == "5") {
      user_profile_menu(access_token);
    } else if (choice == "6") {
      library_menu(access_token);
    } else if (choice == "q" || choice == "Q") {
      std::cout << "Exiting Spotify TUI. Goodbye!" << std::endl;
      break;
    } else {
      std::cout << "Invalid option. Please try again." << std::endl;
    }
  }

  return 0;
}

// ------------------ Playlist Menu ------------------
void playlist_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Playlists Menu ---\n";
    std::cout << "1. View Playlists\n";
    std::cout << "2. Create New Playlist\n";
    std::cout << "3. Add Tracks to Playlist\n";
    std::cout << "b. Back to Main Menu\n";
    std::cout << "Select an option: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
      rapidjson::Document playlists;
      if (get_user_playlists(access_token, playlists)) {
        display_playlists(playlists);
      } else {
        std::cerr << "Failed to retrieve playlists." << std::endl;
      }
    } else if (choice == "2") {
      std::cout << "Enter the name for the new playlist: ";
      std::string playlist_name;
      std::getline(std::cin, playlist_name);

      // You need to get the user ID first
      rapidjson::Document user_profile;
      if (!get_user_profile(access_token, user_profile)) {
        std::cerr << "Failed to retrieve user profile." << std::endl;
        continue;
      }

      std::string user_id;
      if (user_profile.HasMember("id") && user_profile["id"].IsString()) {
        user_id = user_profile["id"].GetString();
      } else {
        std::cerr << "User ID not found." << std::endl;
        continue;
      }

      rapidjson::Document new_playlist;
      if (create_playlist(access_token, user_id, playlist_name, new_playlist)) {
        std::cout << "Playlist \"" << playlist_name
                  << "\" created successfully." << std::endl;
      } else {
        std::cerr << "Failed to create playlist." << std::endl;
      }
    } else if (choice == "3") {
      std::cout << "Enter the Playlist ID to add tracks to: ";
      std::string playlist_id;
      std::getline(std::cin, playlist_id);

      std::cout << "Enter track URIs separated by commas: ";
      std::string track_input;
      std::getline(std::cin, track_input);
      std::vector<std::string> track_uris;
      size_t pos = 0;
      while ((pos = track_input.find(',')) != std::string::npos) {
        std::string token = track_input.substr(0, pos);
        track_uris.push_back(token);
        track_input.erase(0, pos + 1);
      }
      if (!track_input.empty()) {
        track_uris.push_back(track_input);
      }

      if (add_tracks_to_playlist(access_token, playlist_id, track_uris)) {
        std::cout << "Tracks added successfully to the playlist." << std::endl;
      } else {
        std::cerr << "Failed to add tracks to the playlist." << std::endl;
      }
    } else if (choice == "b" || choice == "B") {
      break; // Return to the main menu
    } else {
      std::cout << "Invalid option. Please try again." << std::endl;
    }
  }
}

// ------------------ Playback Control Menu ------------------
void playback_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Playback Control Menu ---\n";
    std::cout << "1. View Current Playback\n";
    std::cout << "2. Play Music\n";
    std::cout << "3. Pause Music\n";
    std::cout << "4. Skip to Next Track\n";
    std::cout << "b. Back to Main Menu\n";
    std::cout << "Select an option: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
      rapidjson::Document playback;
      if (get_current_playback(access_token, playback)) {
        display_current_playback(playback);
      } else {
        std::cerr << "Failed to retrieve current playback." << std::endl;
      }
    } else if (choice == "2") {
      if (play_music(access_token)) {
        std::cout << "Music started playing." << std::endl;
      } else {
        std::cerr << "Failed to start playback." << std::endl;
      }
    } else if (choice == "3") {
      if (pause_music(access_token)) {
        std::cout << "Music paused." << std::endl;
      } else {
        std::cerr << "Failed to pause playback." << std::endl;
      }
    } else if (choice == "4") {
      if (next_track(access_token)) {
        std::cout << "Skipped to the next track." << std::endl;
      } else {
        std::cerr << "Failed to skip track." << std::endl;
      }
    } else if (choice == "b" || choice == "B") {
      break; // Return to the main menu
    } else {
      std::cout << "Invalid option. Please try again." << std::endl;
    }
  }
}

// ------------------ Recommendations Menu ------------------
void recommendations_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Recommendations Menu ---\n";
    std::cout << "Enter seed artist, track, or genre for recommendations: ";

    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> seed_artists = {input}; // Use artist as seed
    rapidjson::Document recommendations;
    if (get_recommendations(access_token, seed_artists, {}, {},
                            recommendations)) {
      display_recommendations(recommendations);
    } else {
      std::cerr << "Failed to retrieve recommendations." << std::endl;
    }

    std::cout << "b. Back to Main Menu\n";
    std::string choice;
    std::getline(std::cin, choice);
    if (choice == "b" || choice == "B") {
      break; // Return to the main menu
    }
  }
}

// ------------------ Search Menu ------------------
void search_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Search Menu ---\n";
    std::cout << "Enter query for search: ";
    std::string query;
    std::getline(std::cin, query);

    rapidjson::Document search_results;
    if (search_spotify(access_token, query, "track", search_results)) {
      display_search_results(search_results, "tracks");
    } else {
      std::cerr << "Failed to perform search." << std::endl;
    }

    std::cout << "b. Back to Main Menu\n";
    std::string choice;
    std::getline(std::cin, choice);
    if (choice == "b" || choice == "B") {
      break; // Return to the main menu
    }
  }
}

// ------------------ User Profile Menu ------------------
void user_profile_menu(const std::string &access_token) {
  rapidjson::Document user_profile;
  if (get_user_profile(access_token, user_profile)) {
    display_user_profile(user_profile);
  } else {
    std::cerr << "Failed to retrieve user profile." << std::endl;
  }
}

// ------------------ Library Menu ------------------
void library_menu(const std::string &access_token) {
  while (true) {
    std::cout << "\n--- Library Menu ---\n";
    std::cout << "1. View Saved Tracks\n";
    std::cout << "2. Save Track to Library\n";
    std::cout << "b. Back to Main Menu\n";
    std::cout << "Select an option: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
      rapidjson::Document saved_tracks;
      if (get_saved_tracks(access_token, saved_tracks)) {
        display_saved_tracks(saved_tracks);
      } else {
        std::cerr << "Failed to retrieve saved tracks." << std::endl;
      }
    } else if (choice == "2") {
      std::cout << "Enter the Track ID to save: ";
      std::string track_id;
      std::getline(std::cin, track_id);

      if (save_track(access_token, track_id)) {
        std::cout << "Track saved to library." << std::endl;
      } else {
        std::cerr << "Failed to save track." << std::endl;
      }
    } else if (choice == "b" || choice == "B") {
      break; // Return to the main menu
    } else {
      std::cout << "Invalid option. Please try again." << std::endl;
    }
  }
}
