//
// Instrumentor.cpp
// Author: Antoine Bastide
// Date: 23/04/2025
//

#include "Engine/Profiling/Instrumentor.hpp"

#include "Engine/Settings.hpp"

namespace Engine::Profiling {
  Instrumentor::Instrumentor() : profileCount(0) {}

  void Instrumentor::beginSession(const std::string &filepath) {
    outputStream.open(filepath);
    writeHeader();
  }

  void Instrumentor::endSession() {
    writeFooter();
    outputStream.close();
    profileCount = 0;
  }

  void Instrumentor::writeHeader() {
    outputStream << "{\"otherData\": {},\"traceEvents\":[";
    outputStream.flush();
  }

  void Instrumentor::writeProfile(const ProfileResult &result) {
    std::lock_guard lock(mutex);

    if (result.level > Settings::Profiling::GetProfilingLevel())
      return;
    const long long duration = result.end - result.start;
    if (duration < Settings::Profiling::GetProfilingThreshold())
      return;

    if (profileCount++ > 0)
      outputStream << ",";

    std::string name = result.name;
    std::ranges::replace(name, '"', '\'');

    outputStream << "{";
    outputStream << "\"dur\":" << duration << ',';
    outputStream << "\"name\":\"" << name << "\",";
    outputStream << "\"ph\":\"X\",";
    outputStream << "\"pid\":0,";
    outputStream << "\"tid\":" << result.threadID << ",";
    outputStream << "\"ts\":" << result.start;
    outputStream << "}";

    outputStream.flush();
  }

  void Instrumentor::writeFooter() {
    outputStream << "]}";
    outputStream.flush();
  }

  Instrumentor &Instrumentor::get() {
    static Instrumentor instance;
    return instance;
  }
}
