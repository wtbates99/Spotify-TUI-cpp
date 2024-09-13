// include/utils.h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Callback function for cURL to write response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

// Encodes a string for use in URLs
std::string url_encode(const std::string &value);

// Trims whitespace from both ends of a string
std::string trim(const std::string &s);

// Splits a string by a delimiter and returns a vector of tokens
std::vector<std::string> split(const std::string &s, char delimiter);

// Gets input from the user with a prompt
std::string get_input(const std::string &prompt);

#endif // UTILS_H
