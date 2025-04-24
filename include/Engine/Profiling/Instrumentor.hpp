//
// Instrumentor.hpp
// Author: Antoine Bastide
// Date: 23/04/2025
//

#ifndef INSTRUMENTOR_HPP
#define INSTRUMENTOR_HPP

#include <fstream>
#include <string>

#include "Engine/Settings.hpp"

namespace Engine::Profiling {
  struct ProfileResult {
    std::string name;
    long long start, end;
    uint32_t threadID;
    Settings::Profiling::ProfilingLevel level;
  };

  struct InstrumentationSession {
    std::string name;
  };

  class Instrumentor {
    public:
      explicit Instrumentor();
      void BeginSession(const std::string &name, const std::string &filepath = "profiling_data.json");
      void EndSession();
      void WriteHeader();
      void WriteProfile(const ProfileResult &result);
      void WriteFooter();
      static Instrumentor &Get();
    private:
      InstrumentationSession *currentSession;
      std::ofstream outputStream;
      int profileCount;
      std::mutex mutex;
  };
}

#endif //INSTRUMENTOR_HPP
