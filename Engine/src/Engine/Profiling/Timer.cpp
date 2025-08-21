//
// Timer.cpp
// Author: Antoine Bastide
// Date: 23/04/2025
//

#include <thread>

#include "Engine/Profiling/Timer.hpp"
#include "Engine/Profiling/Instrumentor.hpp"

namespace Engine::Profiling {
  Timer::Timer(const char *name, const Settings::Profiling::Level level)
    : name(name), level(level), startTimepoint(std::chrono::high_resolution_clock::now()) {}

  Timer::~Timer() {
    const auto endTimepoint = std::chrono::high_resolution_clock::now();
    const long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().
      count();
    const long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
    const uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Instrumentor::get().writeProfile({name, start, end, threadID, level});
  }
}
