// src/utils.cpp
#include "utils.h"
#include <algorithm>
#include <cctype>
#include <curl/curl.h>
#include <iostream>
#include <sstream>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t totalSize = size * nmemb;
  std::string *str = static_cast<std::string *>(userp);
  str->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

std::string url_encode(const std::string &value) {
  CURL *curl = curl_easy_init();
  if (curl) {
    char *output =
        curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
    if (output) {
      std::string encoded(output);
      curl_free(output);
      curl_easy_cleanup(curl);
      return encoded;
    }
    curl_easy_cleanup(curl);
  }
  return "";
}

std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  if (start == std::string::npos)
    return "";
  return s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream ss(s);
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

std::string get_input(const std::string &prompt) {
  std::cout << prompt;
  std::string input;
  std::getline(std::cin, input);
  return trim(input);
}
