// src/spotify_auth.cpp
#include "spotify_auth.h"
#include "base64/base64.h"
#include "rapidjson/document.h"
#include "utils.h"
#include <curl/curl.h>
#include <iostream>
#include <map>
#include <sstream>

size_t WriteCallbackAuth(void *contents, size_t size, size_t nmemb,
                         void *userp) {
  size_t totalSize = size * nmemb;
  std::string *response = static_cast<std::string *>(userp);
  response->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

bool parse_query(const std::string &query,
                 std::map<std::string, std::string> &params) {
  std::stringstream ss(query);
  std::string item;
  while (std::getline(ss, item, '&')) {
    size_t pos = item.find('=');
    if (pos != std::string::npos) {
      std::string key = item.substr(0, pos);
      std::string value = item.substr(pos + 1);
      params[key] = value;
    }
  }
  return true;
}

bool authenticate(std::string &access_token) {
  std::string client_id = get_input("Enter your Spotify Client ID: ");
  std::string client_secret = get_input("Enter your Spotify Client Secret: ");
  std::string redirect_uri = "http://localhost:5000/callback";

  std::string auth_url =
      "https://accounts.spotify.com/authorize?response_type=code&client_id=" +
      url_encode(client_id) +
      "&scope=playlist-modify-public%20playlist-modify-private%20user-read-"
      "playback-state%20user-modify-playback-state&redirect_uri=" +
      url_encode(redirect_uri);

  std::cout
      << "\nPlease open the following URL in your browser to authorize the "
         "application:\n"
      << auth_url
      << "\n\nAfter authorization, you will be redirected to a URL.\n"
      << "Please copy the 'code' parameter from that URL and paste it below.\n";

  std::string auth_code = get_input("Enter the authorization code: ");

  std::string token_url = "https://accounts.spotify.com/api/token";
  std::string post_fields =
      "grant_type=authorization_code&code=" + url_encode(auth_code) +
      "&redirect_uri=" + url_encode(redirect_uri);

  std::string credentials = client_id + ":" + client_secret;
  std::string encoded_credentials = base64_encode(
      reinterpret_cast<const unsigned char *>(credentials.c_str()),
      credentials.length());

  CURL *curl_token = curl_easy_init();
  if (curl_token) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(
        headers, ("Authorization: Basic " + encoded_credentials).c_str());

    std::string response;
    curl_easy_setopt(curl_token, CURLOPT_URL, token_url.c_str());
    curl_easy_setopt(curl_token, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_token, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl_token, CURLOPT_WRITEFUNCTION, WriteCallbackAuth);
    curl_easy_setopt(curl_token, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_token);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl_token);

    if (res != CURLE_OK) {
      return false;
    }

    rapidjson::Document doc;
    if (doc.Parse(response.c_str()).HasParseError()) {
      return false;
    }

    if (doc.HasMember("access_token") && doc["access_token"].IsString()) {
      access_token = doc["access_token"].GetString();
      return true;
    }
  }
  return false;
}
