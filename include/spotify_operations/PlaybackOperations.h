#ifndef PLAYBACK_OPERATIONS_H
#define PLAYBACK_OPERATIONS_H

#include <string>

void playback_menu(const std::string &access_token);
bool play_music(const std::string &access_token);
bool pause_music(const std::string &access_token);
bool skip_track(const std::string &access_token);
bool set_volume(const std::string &access_token, int volume);
bool toggle_shuffle(const std::string &access_token, bool enable);
bool toggle_repeat(const std::string &access_token, const std::string &state);

#endif // PLAYBACK_OPERATIONS_H
