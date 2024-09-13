
// src/main.cpp
#include "spotify_auth.h"
#include "spotify_operations/LibraryOperations.h"
#include "spotify_operations/PlaybackOperations.h"
#include "spotify_operations/PlaylistOperations.h"
#include "spotify_operations/RecommendationsOperations.h"
#include "spotify_operations/SearchOperations.h"
#include "utils.h"
#include <iostream>
#include <string>

// ANSI escape codes for colors and formatting
#define RESET "\033[0m"
#define BOLD "\033[1m"

#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"

// Function to clear the console screen
void clear_screen() { std::cout << "\033[2J\033[1;1H"; }

// Function to display the header
void display_header() {
  std::cout << FG_CYAN << BOLD
            << "==========================================" << RESET
            << std::endl;
  std::cout << FG_CYAN << BOLD << "          Spotify TUI Application          "
            << RESET << std::endl;
  std::cout << FG_CYAN << BOLD
            << "==========================================" << RESET
            << std::endl;
}

// Function to display the main menu
void display_main_menu() {
  std::cout << FG_GREEN << BOLD << "\n=== Main Menu ===" << RESET << std::endl;
  std::cout << "1. Playlists" << std::endl;
  std::cout << "2. Playback Control" << std::endl;
  std::cout << "3. Recommendations" << std::endl;
  std::cout << "4. Search" << std::endl;
  std::cout << "5. Library Management" << std::endl;
  std::cout << "q. Quit" << std::endl;
  std::cout << FG_YELLOW << "Select an option: " << RESET;
}

// Function to authenticate and retrieve access token
bool authenticate_user(std::string &access_token) {
  return authenticate(access_token);
}

// Function to handle invalid input
void handle_invalid_input() {
  std::cout << FG_RED << "Invalid input. Please try again." << RESET
            << std::endl;
}

int main() {
  std::string access_token;
  clear_screen();
  display_header();
  std::cout << "\nWelcome to the " << FG_MAGENTA << "Spotify TUI" << RESET
            << " Application!\n"
            << std::endl;

  std::cout << "Authenticating...";
  if (!authenticate_user(access_token)) {
    std::cerr << FG_RED
              << "\nAuthentication failed. Please check your credentials and "
                 "try again."
              << RESET << std::endl;
    return 1;
  }
  std::cout << FG_GREEN << " Success!" << RESET << std::endl;

  while (true) {
    display_header();
    display_main_menu();

    std::string choice;
    std::getline(std::cin, choice);
    choice = trim(choice);

    if (choice.empty()) {
      handle_invalid_input();
      continue;
    }

    if (choice == "1") {
      playlist_menu(access_token);
    } else if (choice == "2") {
      playback_menu(access_token);
    } else if (choice == "3") {
      recommendations_menu(access_token);
    } else if (choice == "4") {
      search_menu(access_token);
    } else if (choice == "5") {
      library_menu(access_token);
    } else if (choice == "q" || choice == "Q") {
      std::cout << FG_BLUE << "Exiting application. Goodbye!" << RESET
                << std::endl;
      break;
    } else {
      handle_invalid_input();
    }
  }

  return 0;
}
