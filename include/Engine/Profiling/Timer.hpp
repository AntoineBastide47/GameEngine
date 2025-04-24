//
// Timer.hpp
// Author: Antoine Bastide
// Date: 23/04/2025
//

#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#include "Engine/Settings.hpp"

namespace Engine::Profiling {
  class Timer {
    public:
      Timer(const char *name, Settings::Profiling::ProfilingLevel level);
      ~Timer();
    private:
      const char *name;
      Settings::Profiling::ProfilingLevel level;
      std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
  };
}

#endif //TIMER_HPP
