//
// Build.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "commands/Build.hpp"
#include "commands/Dependencies.hpp"

Build::Build()
  : Command(
    "builds the engine's debug, debug with profile and release static libraries", "{|true}", 1, "", true,
    "The release static library can be omitted when building to save time, add the argument 'true' to build it."
  ) {}

void Build::Run(const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands) {
  const std::string &buildRelease = args[0];

  if (!args.empty() && buildRelease != "true") {
    std::cout << RED << R"(Unexpected input, expected: "" or "true", got: )" << buildRelease << "\n" << "Try: "
      << CLI_EXECUTE_COMMAND(COMMAND_HELP) << " for more information." << RESET;
    return;
  }

  // Update the dependencies
  commands[COMMAND_DEPENDENCIES]->Run(args, commands);

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


  // Debug
  std::system("cmake -B build/debug -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=0 -Wdev -Wdeprecated --log-level=WARNING");
  std::system("cmake --build build/debug");

  // Debug + Profiling
  std::system(
    "cmake -B build/debug-prof -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=1 -Wdev -Wdeprecated --log-level=WARNING"
  );
  std::system("cmake --build build/debug-prof");

  // Release
  if (buildRelease == "true") {
    std::system("cmake -E rm -rf build/release");
    std::system(
      "cmake -B build/release -S . -DCMAKE_BUILD_TYPE=Release -DPROFILE=0 -Wdev -Wdeprecated --log-level=WARNING"
    );
    std::system("cmake --build build/release");
  }
}
