// include/base64/base64.h
#ifndef BASE64_H
#define BASE64_H

#include <string>

// Encodes a standard string to Base64
std::string base64_encode(const std::string &in);

// Encodes a byte array to Base64
std::string base64_encode(const unsigned char *data, size_t len);

// Decodes a Base64 encoded string
std::string base64_decode(const std::string &in);

// Decodes a Base64 encoded byte array
std::string base64_decode(const char *data, size_t len);

#endif // BASE64_H
