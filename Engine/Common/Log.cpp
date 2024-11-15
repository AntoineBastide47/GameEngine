//
// Log.cpp
// Author: Antoine Bastide
// Date: 14/11/2024
//

#include "Common/Log.h"

#include <iostream>
#include <ostream>
#include <cpptrace/cpptrace.hpp>

namespace Engine {
  void Log::Message(const std::string &msg, const bool showTrace) { print(msg, showTrace); }

  void Log::Warning(const std::string &msg, const std::string &funcSignature) {
    print("WARNING::" + extractFromFuncSignature(funcSignature) + ": " + msg);
  }

  std::nullptr_t Log::Error(const std::string &msg, const std::string &funcSignature) {
    print("ERROR::" + extractFromFuncSignature(funcSignature) + ": " + msg);
    return nullptr;
  }

  void Log::print(const std::string &msg, const bool showTrace) {
    std::cout << std::endl << msg << std::endl;
    if (showTrace)
      cpptrace::generate_trace().print();
    std::cout << std::endl;
  }

  std::string Log::extractFromFuncSignature(const std::string &funcSignature) {
    const std::regex functionRegex(R"((\w+(::\w+)+::(?:operator\W*|\w+))(?=\())");
    if (std::smatch match; std::regex_search(funcSignature, match, functionRegex) && !match.empty())
      return match.str(0);
    return funcSignature;
  }
}
