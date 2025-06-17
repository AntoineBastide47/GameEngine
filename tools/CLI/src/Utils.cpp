//
// Utils.cpp
// Author: Antoine Bastide
// Date: 03/04/2025
//

#include <sstream>

#include "Utils.hpp"

std::string Utils::Join(const std::vector<std::string> &v, const std::string &separator) {
  std::ostringstream oss;

  for (size_t i = 0; i < v.size(); ++i) {
    oss << v[i];
    if (i != v.size() - 1)
      oss << separator;
  }

  return oss.str();
}

std::vector<std::string> Utils::Split(const std::string &s, const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end;
  while ((end = s.find(delimiter, start)) != std::string::npos) {
    tokens.push_back(s.substr(start, end - start));
    start = end + delimiter.length();
  }
  tokens.push_back(s.substr(start));
  return tokens;
}

std::string Utils::Trim(const std::string &str) {
  const auto start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos) return ""; // all whitespace

  const auto end = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(start, end - start + 1);
}
