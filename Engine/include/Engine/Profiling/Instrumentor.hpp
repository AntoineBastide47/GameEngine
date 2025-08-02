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

namespace Engine2D {
  class Game2D;
}

namespace Engine::Profiling {
  struct ProfileResult final {
    std::string name;
    long long start, end;
    uint32_t threadID;
    Settings::Profiling::ProfilingLevel level;
  };

  class Instrumentor final {
    friend class Timer;
    friend class Engine2D::Game2D;

    /// The output stream that points to the file we are writing to
    std::ofstream outputStream;
    /// The number of timing profiles that has been written
    int profileCount;
    std::mutex mutex;

    explicit Instrumentor();

    /// Starts the instrumentor
    void beginSession(const std::string &filepath = "profiling_data.json");
    /// Stops the instrumentor
    void endSession();

    /// Writes the header of the output file
    void writeHeader();
    /// Writes a profile in the output file
    void writeProfile(const ProfileResult &result);
    /// Writes the footer of the output file
    void writeFooter();

    /// Returns the current instrumentor
    static Instrumentor &get();
  };
}

#endif //INSTRUMENTOR_HPP
