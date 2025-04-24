//
// Instrumentor.cpp
// Author: Antoine Bastide
// Date: 23/04/2025
//

#include "Engine/Profiling/Instrumentor.hpp"

#include "Engine/Settings.hpp"
#include "Engine/Macros/Utils.hpp"

namespace Engine::Profiling {
  Instrumentor::Instrumentor() : currentSession(nullptr), profileCount(0) {}

  void Instrumentor::BeginSession(const std::string &name, const std::string &filepath) {
    outputStream.open(filepath);
    WriteHeader();
    currentSession = new InstrumentationSession{name};
  }

  void Instrumentor::EndSession() {
    WriteFooter();
    outputStream.close();
    SAFE_DELETE(currentSession);
    profileCount = 0;
  }

  void Instrumentor::WriteHeader() {
    outputStream << "{\"otherData\": {},\"traceEvents\":[";
    outputStream.flush();
  }

  void Instrumentor::WriteProfile(const ProfileResult &result) {
    std::lock_guard lock(mutex);

    if (Settings::Profiling::GetProfilingLevel() > result.level)
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

  void Instrumentor::WriteFooter() {
    outputStream << "]}";
    outputStream.flush();
  }

  Instrumentor &Instrumentor::Get() {
    static Instrumentor instance;
    return instance;
  }
}
