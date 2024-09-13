// spotify_operations.h
#ifndef SPOTIFY_OPERATIONS_H
#define SPOTIFY_OPERATIONS_H

#include "rapidjson/document.h"
#include <string>
#include <utility>
#include <vector>

// ------------------ Playlist Operations ------------------

// Retrieves the current user's playlists.
bool get_user_playlists(const std::string &access_token,
                        rapidjson::Document &playlists);

// Displays the list of playlists.
void display_playlists(const rapidjson::Document &playlists);

// Retrieves details of a specific playlist by its ID.
bool get_playlist_details(const std::string &access_token,
                          const std::string &playlist_id,
                          rapidjson::Document &playlist_details);

// Displays detailed information about a playlist, including its tracks.
void display_playlist_details(const rapidjson::Document &playlist_details);

// Creates a new playlist for the user.
bool create_playlist(const std::string &access_token,
                     const std::string &user_id,
                     const std::string &playlist_name,
                     rapidjson::Document &new_playlist);

// Adds tracks to a specified playlist.
bool add_tracks_to_playlist(const std::string &access_token,
                            const std::string &playlist_id,
                            const std::vector<std::string> &track_uris);

// ------------------ Playback Control ------------------

// Retrieves the current playback state.
bool get_current_playback(const std::string &access_token,
                          rapidjson::Document &playback);

// Displays information about the currently playing track.
void display_current_playback(const rapidjson::Document &playback);

// Starts playback on the user's active device.
bool play_music(const std::string &access_token,
                const std::string &device_id = "");

// Pauses playback on the user's active device.
bool pause_music(const std::string &access_token,
                 const std::string &device_id = "");

// Skips to the next track in the user's queue.
bool next_track(const std::string &access_token,
                const std::string &device_id = "");

// ------------------ Recommendations ------------------

// Retrieves track recommendations based on seed artists, tracks, and genres.
bool get_recommendations(const std::string &access_token,
                         const std::vector<std::string> &seed_artists,
                         const std::vector<std::string> &seed_tracks,
                         const std::vector<std::string> &seed_genres,
                         rapidjson::Document &recommendations, int limit = 20);

// Displays a list of recommended tracks.
void display_recommendations(const rapidjson::Document &recommendations);

// ------------------ Search ------------------

// Searches Spotify for tracks, artists, albums, etc.
bool search_spotify(const std::string &access_token, const std::string &query,
                    const std::string &type,
                    rapidjson::Document &search_results, int limit = 20);

// Displays search results based on the type of search.
void display_search_results(const rapidjson::Document &search_results,
                            const std::string &type);

// ------------------ User Information ------------------

// Retrieves the current user's profile information.
bool get_user_profile(const std::string &access_token,
                      rapidjson::Document &user_profile);

// Displays the user's profile information.
void display_user_profile(const rapidjson::Document &user_profile);

// ------------------ Library Management ------------------

// Retrieves the user's saved tracks.
bool get_saved_tracks(const std::string &access_token,
                      rapidjson::Document &saved_tracks, int limit = 20);

// Displays the user's saved tracks.
void display_saved_tracks(const rapidjson::Document &saved_tracks);

// Saves a specific track to the user's library.
bool save_track(const std::string &access_token, const std::string &track_id);

// ------------------ Additional Operations ------------------

// Add any additional function prototypes here as needed for other Spotify API
// operations.

#endif // SPOTIFY_OPERATIONS_H
