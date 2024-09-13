#include "rapidjson/document.h"
#include "spotify_auth.h"
#include "spotify_operations.h"
#include "utils.h"
#include <iostream>
#include <string>

int main() {
  std::string access_token;

  if (!authenticate(access_token)) {
    std::cerr << "Authentication failed." << std::endl;
    return 1;
  }

  rapidjson::Document playlists;
  if (!get_user_playlists(access_token, playlists)) {
    std::cerr << "Failed to get playlists." << std::endl;
    return 1;
  }

  display_playlists(playlists);

  return 0;
}
