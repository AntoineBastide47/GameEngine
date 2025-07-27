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
  class Timer final {
    public:
      /// Starts a new RAII timer with the given name and profiling level
      Timer(const char *name, Settings::Profiling::ProfilingLevel level);
      /// Stops the timer
      ~Timer();
    private:
      /// The name of the timer
      const char *name;
      /// The profiling level of this timer
      Settings::Profiling::ProfilingLevel level;
      /// The time at which the timer started
      std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
  };
}

#endif //TIMER_HPP
