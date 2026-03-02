#ifndef BASE64_H
#define BASE64_H

#include <string>

// Encode raw bytes to base64
std::string base64_encode(const unsigned char* data, size_t len, bool url = false);

// Decode base64 string to raw string
std::string base64_decode(const std::string& encoded, bool remove_linebreaks = false);

#endif // BASE64_H