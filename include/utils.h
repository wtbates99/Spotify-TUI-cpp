#ifndef UTILS_H
#define UTILS_H

#include <string>

// Utility function prototypes
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
std::string url_encode(const std::string &value);
std::string get_input(const std::string &prompt);
std::string base64_encode(const std::string &in);

#endif // UTILS_H
