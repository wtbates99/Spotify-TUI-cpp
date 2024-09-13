#include "spotify_operations.h"
#include "utils.h"
#include <iostream>
#include <curl/curl.h>
#include "rapidjson/error/en.h"
#include <vector>

bool get_user_playlists(const std::string& access_token, rapidjson::Document& playlists) {
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        std::string auth_header = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_header.c_str());

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/playlists");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        // Parse JSON response
        if (playlists.Parse(response.c_str()).HasParseError()) {
            std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(playlists.GetParseError()) << std::endl;
            return false;
        }

        return true;
    }
    return false;
}

void display_playlists(const rapidjson::Document& playlists) {
    if (!playlists.HasMember("items") || !playlists["items"].IsArray()) {
        std::cerr << "No playlists found." << std::endl;
        return;
    }

    const rapidjson::Value& items = playlists["items"];
    std::vector<std::string> playlist_names;

    for (rapidjson::SizeType i = 0; i < items.Size(); ++i) {
        const rapidjson::Value& item = items[i];
        if (item.HasMember("name") && item["name"].IsString()) {
            playlist_names.push_back(item["name"].GetString());
        }
    }

    // Display playlists
    std::cout << "\nYour Playlists:\n";
    for (size_t i = 0; i < playlist_names.size(); ++i) {
        std::cout << i + 1 << ". " << playlist_names[i] << std::endl;
    }

    std::cout << "\nEnter the number of the playlist to view details, or 'q' to quit: ";
    std::string input;
    getline(std::cin, input);

    if (input == "q" || input == "Q") {
        return;
    }

    int choice = std::stoi(input);
    if (choice > 0 && choice <= static_cast<int>(playlist_names.size())) {
        std::cout << "You selected: " << playlist_names[choice - 1] << std::endl;
        // Additional code to display playlist details can be added here
    } else {
        std::cout << "Invalid selection." << std::endl;
    }
}
