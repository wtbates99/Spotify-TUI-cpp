#include "spotify_auth.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "utils.h"
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <fstream>
#include <iostream>

bool authenticate(std::string &access_token) {
  const char *client_id_env = std::getenv("SPOTIFY_CLIENT_ID");
  const char *client_secret_env = std::getenv("SPOTIFY_CLIENT_SECRET");
  std::string client_id = client_id_env ? client_id_env : "";
  std::string client_secret = client_secret_env ? client_secret_env : "";

  if (client_id.empty() || client_secret.empty()) {
    std::cerr << "Error: Please set SPOTIFY_CLIENT_ID and "
                 "SPOTIFY_CLIENT_SECRET environment variables."
              << std::endl;
    return false;
  }

  std::string refresh_token;
  time_t expires_at = 0;

  // Try to load tokens from file
  if (load_tokens(access_token, refresh_token, expires_at)) {
    time_t current_time = std::time(nullptr);
    if (current_time < expires_at) {
      // Access token is still valid
      return true;
    } else {
      // Access token has expired, refresh it
      time_t expires_in;
      if (refresh_access_token(refresh_token, client_id, client_secret,
                               access_token, expires_in)) {
        // Save new tokens
        save_tokens(access_token, refresh_token, expires_in);
        return true;
      } else {
        std::cerr << "Failed to refresh access token." << std::endl;
        // Proceed to full authentication flow
      }
    }
  }

  // If tokens are not available or invalid, proceed with authorization code
  // flow
  std::string redirect_uri = "http://localhost:5000/callback";

  // Step 1: Direct user to authorization URL
  std::string auth_url = "https://accounts.spotify.com/authorize";
  auth_url += "?client_id=" + url_encode(client_id);
  auth_url += "&response_type=code";
  auth_url += "&redirect_uri=" + url_encode(redirect_uri);
  auth_url += "&scope=" + url_encode("playlist-read-private");
  auth_url += "&show_dialog=false";

  std::cout << "Please open the following URL in your browser and authorize "
               "the application:\n"
            << auth_url << std::endl;

  // Step 2: Get authorization code from user
  std::string code = get_input("Enter the code from the URL: ");

  // Step 3: Exchange authorization code for access token
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string post_fields = "grant_type=authorization_code";
    post_fields += "&code=" + url_encode(code);
    post_fields += "&redirect_uri=" + url_encode(redirect_uri);

    // Prepare client credentials for Basic authentication
    std::string credentials = client_id + ":" + client_secret;
    std::string encoded_credentials = base64_encode(credentials);

    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Basic " + encoded_credentials;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(
        headers, "Content-Type: application/x-www-form-urlencoded");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL,
                     "https://accounts.spotify.com/api/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
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
    rapidjson::Document json_response;
    if (json_response.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(json_response.GetParseError())
                << std::endl;
      return false;
    }

    if (json_response.HasMember("access_token") &&
        json_response.HasMember("refresh_token")) {
      access_token = json_response["access_token"].GetString();
      refresh_token = json_response["refresh_token"].GetString();
      time_t expires_in = json_response["expires_in"].GetInt64();

      // Save tokens to file
      save_tokens(access_token, refresh_token, expires_in);

      return true;
    } else {
      std::cerr << "Error in response: " << response << std::endl;
      return false;
    }
  }
  return false;
}

void save_tokens(const std::string &access_token,
                 const std::string &refresh_token, time_t expires_in) {
  std::ofstream ofs("tokens.json");
  if (!ofs.is_open()) {
    std::cerr << "Failed to open tokens.json for writing." << std::endl;
    return;
  }

  ofs << "{\n";
  ofs << "  \"access_token\": \"" << access_token << "\",\n";
  ofs << "  \"refresh_token\": \"" << refresh_token << "\",\n";
  ofs << "  \"expires_at\": " << (std::time(nullptr) + expires_in) << "\n";
  ofs << "}\n";
}

bool load_tokens(std::string &access_token, std::string &refresh_token,
                 time_t &expires_at) {
  std::ifstream ifs("tokens.json");
  if (!ifs.is_open()) {
    return false;
  }

  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  rapidjson::Document doc;
  if (doc.Parse(content.c_str()).HasParseError()) {
    return false;
  }

  if (doc.HasMember("access_token") && doc["access_token"].IsString()) {
    access_token = doc["access_token"].GetString();
  } else {
    return false;
  }

  if (doc.HasMember("refresh_token") && doc["refresh_token"].IsString()) {
    refresh_token = doc["refresh_token"].GetString();
  } else {
    return false;
  }

  if (doc.HasMember("expires_at") && doc["expires_at"].IsInt64()) {
    expires_at = doc["expires_at"].GetInt64();
  } else {
    return false;
  }

  return true;
}

bool refresh_access_token(const std::string &refresh_token,
                          const std::string &client_id,
                          const std::string &client_secret,
                          std::string &access_token, time_t &expires_in) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string post_fields = "grant_type=refresh_token";
    post_fields += "&refresh_token=" + url_encode(refresh_token);

    std::string credentials = client_id + ":" + client_secret;
    std::string encoded_credentials = base64_encode(credentials);

    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Basic " + encoded_credentials;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(
        headers, "Content-Type: application/x-www-form-urlencoded");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL,
                     "https://accounts.spotify.com/api/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
      std::cerr << "Curl Error: " << curl_easy_strerror(res) << std::endl;
      return false;
    }

    // Parse JSON response
    rapidjson::Document json_response;
    if (json_response.Parse(response.c_str()).HasParseError()) {
      std::cerr << "JSON parse error: "
                << rapidjson::GetParseError_En(json_response.GetParseError())
                << std::endl;
      return false;
    }

    if (json_response.HasMember("access_token")) {
      access_token = json_response["access_token"].GetString();
      expires_in = json_response["expires_in"].GetInt64();
      return true;
    } else {
      std::cerr << "Error in response: " << response << std::endl;
      return false;
    }
  }
  return false;
}
