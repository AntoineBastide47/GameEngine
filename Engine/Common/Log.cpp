//
// Log.cpp
// Author: Antoine Bastide
// Date: 14/11/2024
//

#include <iostream>
#include <ostream>
#include <cpptrace/cpptrace.hpp>

#include "Common/Log.h"

namespace Engine {
  void Log::Print(const std::string &message) {
    std::cout << message;
  }

  void Log::Println(const std::string &message) {
    std::cout << message << std::endl;
  }

  void Log::Print(const char *&message) {
    std::cout << message;
  }

  void Log::Println(const char *&message) {
    std::cout << message << std::endl;
  }

  void Log::Message(const std::string &msg, const std::string &funcSignature, const bool showTrace) {
    show("MESSAGE::" + extractFromFuncSignature(funcSignature) + ": " + msg, showTrace);
  }

  void Log::Warning(const std::string &msg, const std::string &funcSignature) {
    show("WARNING::" + extractFromFuncSignature(funcSignature) + ": " + msg);
  }

  std::nullptr_t Log::Error(const std::string &msg, const std::string &funcSignature) {
    show("ERROR::" + extractFromFuncSignature(funcSignature) + ": " + msg);
    return nullptr;
  }

  void Log::show(const std::string &msg, const bool showTrace) {
    std::cout << std::endl << msg << std::endl;
    if (showTrace) {
      cpptrace::generate_trace().print();
      std::cout << std::endl;
    }
  }

  std::string Log::extractFromFuncSignature(const std::string &funcSignature) {
    const std::regex functionRegex(R"((\w+(::\w+)+::(?:operator\W*|\w+))(?=\())");
    if (std::smatch match; std::regex_search(funcSignature, match, functionRegex) && !match.empty())
      return match.str(0);
    return funcSignature;
  }
}
