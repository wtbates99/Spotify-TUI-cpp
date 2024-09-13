#include "utils.h"
#include <iostream>
#include <curl/curl.h>

// Callback function for libcurl to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    ((std::string*)userp)->append((char*)contents, real_size);
    return real_size;
}

// URL encoding function
std::string url_encode(const std::string& value) {
    CURL* curl = curl_easy_init();
    std::string result;
    if (curl) {
        char* output = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
        if (output) {
            result = output;
            curl_free(output);
        }
        curl_easy_cleanup(curl);
    }
    return result;
}

// Function to get user input
std::string get_input(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    getline(std::cin, input);
    return input;
}

// Base64 encoding function
std::string base64_encode(const std::string& in) {
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;

    int val = 0;
    int valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (out.size() % 4) {
        out.push_back('=');
    }

    return out;
}
