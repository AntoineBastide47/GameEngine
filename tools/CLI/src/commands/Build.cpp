//
// Build.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include <string>
#include <sys/sysctl.h>

#include "commands/Build.hpp"

#define ARGS " -Wdev -Wdeprecated --log-level=WARNING "
#define BUILD_ENGINE "-DBUILD_ENGINE=ON -DBUILD_EDITOR=OFF"
#define BUILD_EDITOR "-DBUILD_ENGINE=OFF -DBUILD_EDITOR=ON"

#define ENGINE_INCLUDE "Engine/include"
#define ENGINE_BUILD "Engine/build"
#define EDITOR_BUILD "Editor/build"

#define DEBUG_CMD "/debug -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=0" ARGS BUILD_ENGINE
#define PROFILING_CMD "/profiling -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=1" ARGS BUILD_ENGINE
#define RELEASE_CMD "/release -S . -DCMAKE_BUILD_TYPE=Release -DPROFILE=0" ARGS BUILD_ENGINE
#define EDITOR_CMD ARGS BUILD_EDITOR

Build::Build(const bool buildEngine, const bool buildEditor)
  : Command(
      [=]() {
        if (buildEngine && buildEditor)
          return "builds the engine's editor and static libraries.";
        if (buildEditor)
          return "builds the engine's editor.";
        return "builds the engine's static libraries.";
      }(), "{|true}", 1, "", true,
      [=]() {
        if (buildEngine)
          return
              "The release static library can be omitted when building to save time, add the argument 'true' to build it.";
        return "";
      }()
    ), buildEngine(buildEngine), buildEditor(buildEditor) {}

void Build::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  const std::string &buildRelease = args[0];

  if (!args.empty() && buildRelease != "true") {
    std::cout << RED << R"(Unexpected input, expected: "" or "true", got: )" << buildRelease << "\n" << "Try: "
        << CLI_EXECUTE_COMMAND(COMMAND_HELP) << " for more information." << RESET;
    return;
  }

  // Remove previously built library
  const std::regex versionedPattern(R"(.*-[0-9]+\.[0-9]+\.[0-9]+(d|dp)?(\.a|\.lib|\.so|\.dll)?)");
  for (const fs::path includeDir = ENGINE_INCLUDE; const auto &entry: fs::directory_iterator(includeDir)) {
    if (const std::string filename = entry.path().filename().string(); std::regex_match(filename, versionedPattern)) {
      if (entry.is_regular_file())
        fs::remove(entry.path());
      else
        fs::remove_all(entry.path());
    }
  }

  const std::string parallel = " --parallel " + std::to_string(GetCoreCount());

  if (buildEngine) {
    // Header Forge
    std::system(
      "./header-forge --parse ./" ENGINE_INCLUDE " --compilerArgs -I./" ENGINE_INCLUDE
      " -I./vendor/glfw/include -I./vendor/glm -I./vendor/cpptrace/include -I./" ENGINE_BUILD "/debug/_cmrc/include"
    );

    // Debug
    std::system(HasNinja() ? "cmake -G Ninja -B " ENGINE_BUILD DEBUG_CMD : "cmake -B " ENGINE_BUILD DEBUG_CMD);
    std::system(("cmake --build " ENGINE_BUILD "/debug" + parallel).c_str());

    // Profiling
    std::system(HasNinja() ? "cmake -G Ninja -B " ENGINE_BUILD PROFILING_CMD : "cmake -B " ENGINE_BUILD PROFILING_CMD);
    std::system(("cmake --build " ENGINE_BUILD "/profiling" + parallel).c_str());

    // Release
    if (buildRelease == "true") {
      std::system("cmake -E rm -rf " ENGINE_BUILD "/release");
      std::system(HasNinja() ? "cmake -G Ninja -B " ENGINE_BUILD RELEASE_CMD : "cmake -B " ENGINE_BUILD RELEASE_CMD);
      std::system(("cmake --build " ENGINE_BUILD "/release" + parallel).c_str());
    }
  }

  if (buildEditor) {
    std::system(HasNinja() ? "cmake -G Ninja -B " EDITOR_BUILD EDITOR_CMD : "cmake -B " EDITOR_BUILD EDITOR_CMD);
    std::system(("cmake --build " EDITOR_BUILD + parallel).c_str());
  }
}

int Build::GetCoreCount() {
  #if defined(_WIN32)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
  #elif defined(__APPLE__)
  int count;
  size_t size = sizeof(count);
  sysctlbyname("hw.ncpu", &count, &size, nullptr, 0);
  return count;
  #elif defined(__linux__)
  return sysconf(_SC_NPROCESSORS_ONLN);
  #else
  return 2;
  #endif
}

bool Build::HasNinja() {
  #if defined(_WIN32)
  return std::system("where ninja >nul 2>&1") == 0;
  #else
  return std::system("command -v ninja >/dev/null 2>&1") == 0;
  #endif
}
