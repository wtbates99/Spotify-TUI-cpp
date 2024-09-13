#ifndef SPOTIFY_AUTH_H
#define SPOTIFY_AUTH_H

#include <ctime>
#include <string>

// Function prototypes for authentication
bool authenticate(std::string &access_token);
void save_tokens(const std::string &access_token,
                 const std::string &refresh_token, time_t expires_in);
bool load_tokens(std::string &access_token, std::string &refresh_token,
                 time_t &expires_at);
bool refresh_access_token(const std::string &refresh_token,
                          const std::string &client_id,
                          const std::string &client_secret,
                          std::string &access_token, time_t &expires_in);

#endif // SPOTIFY_AUTH_H
