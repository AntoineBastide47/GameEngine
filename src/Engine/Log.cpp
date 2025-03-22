//
// Log.cpp
// Author: Antoine Bastide
// Date: 14/11/2024
//

#include <iostream>
#include <ostream>
#include <cpptrace/cpptrace.hpp>

#include "Engine/Log.hpp"

namespace Engine {
  std::vector<int64_t> Log::messagesSent;

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

  void Log::Message(const std::string &msg, const bool showTrace) {
    show("MESSAGE::" + extractFromFuncSignature(__PRETTY_FUNCTION__) + ": " + msg, showTrace);
  }

  void Log::Warning(const std::string &msg) {
    show("WARNING::" + extractFromFuncSignature(__PRETTY_FUNCTION__) + ": " + msg);
  }

  std::nullptr_t Log::Error(const std::string &msg) {
    show("ERROR::" + extractFromFuncSignature(__PRETTY_FUNCTION__) + ": " + msg);
    return nullptr;
  }

  void Log::show(const std::string &msg, const bool showTrace) {
    const auto hash = hashStringToInt64(msg);
    if (std::ranges::find(messagesSent, hash) != messagesSent.end())
      return;

    messagesSent.emplace_back(hash);
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

  int64_t Log::hashStringToInt64(const std::string &msg) {
    constexpr std::hash<std::string> hasher;
    return hasher(msg);
  }
}
