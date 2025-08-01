//
// Build.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include <string>
#include <sys/sysctl.h>

#include "commands/Build.hpp"

Build::Build()
  : Command(
    "build's the engine's debug, debug with profile and release static libraries", "{|true}", 1, "", true,
    "The release static library can be omitted when building to save time, add the argument 'true' to build it."
  ) {}

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
  for (const fs::path includeDir = "include"; const auto &entry: fs::directory_iterator(includeDir)) {
    if (const std::string filename = entry.path().filename().string(); std::regex_match(filename, versionedPattern)) {
      if (entry.is_regular_file())
        fs::remove(entry.path());
      else
        fs::remove_all(entry.path());
    }
  }

  // Header Forge
  std::system(
    "./header-forge --parse ./include --compilerArgs -I./include -I./vendor/glfw/include -I./vendor/glm "
    "-I./vendor/cpptrace/include -I./build/debug/_cmrc/include"
  );

  const std::string parallel = " --parallel " + std::to_string(GetCoreCount());

  // Debug
  std::system(
    HasNinja()
      ? "cmake -G Ninja -B build/debug -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=0 -Wdev -Wdeprecated --log-level=WARNING"
      : "cmake -B build/debug -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=0 -Wdev -Wdeprecated --log-level=WARNING"
  );
  std::system(("cmake --build build/debug" + parallel).c_str());

  // Debug + Profiling
  std::system(
    HasNinja()
      ? "cmake -G Ninja -B build/profiling -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=1 -Wdev -Wdeprecated --log-level=WARNING"
      : "cmake -B build/profiling -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=1 -Wdev -Wdeprecated --log-level=WARNING"
  );
  std::system(("cmake --build build/profiling" + parallel).c_str());

  // Release
  if (buildRelease == "true") {
    std::system("cmake -E rm -rf build/release");
    std::system(
      HasNinja()
        ? "cmake -G Ninja -B build/release -S . -DCMAKE_BUILD_TYPE=Release -DPROFILE=0 -Wdev -Wdeprecated --log-level=WARNING"
        : "cmake -B build/release -S . -DCMAKE_BUILD_TYPE=Release -DPROFILE=0 -Wdev -Wdeprecated --log-level=WARNING"
    );
    std::system(("cmake --build build/release" + parallel).c_str());
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
