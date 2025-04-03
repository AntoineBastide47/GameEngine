//
// Utils.hpp
// Author: Antoine Bastide
// Date: 03/04/2025
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

class Utils {
  public:
    static std::string Join(const std::vector<std::string> &v, const std::string &separator);
    static std::vector<std::string> Split(const std::string &s, const std::string &delimiter);
    static std::string Trim(const std::string &str);
};

#endif //UTILS_HPP
