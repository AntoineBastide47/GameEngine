//
// Build.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "commands/Build.hpp"
#include "commands/Dependencies.hpp"

Build::Build() : Command("builds the engine's static library in debug or release mode", "{debug|release}", 1, "debug") {}

void Build::Run(const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands) {
  const std::string &buildType = args[0];
  if (buildType != "debug" && buildType != "release") {
    std::cout << RED << "Unknown build type: " << buildType << "\n" << "Try '" << CLI_COMMAND
      << " help build' for more information." << RESET;
    return;
  }

  // Since build-project also uses this command, only update the dependencies from the engine
  if (args.size() == 1) {
    commands[COMMAND_DEPENDENCIES]->Run(args, commands);

    // Remove previously built library
    const std::regex versionedPattern(R"(.*-[0-9]+\.[0-9]+\.[0-9]+.*)");
    for (const fs::path includeDir = "include"; const auto &entry: fs::directory_iterator(includeDir))
      if (const std::string filename = entry.path().filename().string(); std::regex_match(filename, versionedPattern))
        fs::remove_all(entry.path());
  } else {
    std::ifstream cmakeFile("CMakeLists.txt");
    if (!cmakeFile) {
      std::cerr << RED << "CMakeLists.txt not found in the current directory." << RESET << std::endl;
      return;
    }

    std::string line;
    std::regex pattern(R"(^\s*project\s*\(\s*([a-zA-Z0-9_\-]+)\b.*\))", std::regex::icase);
    std::smatch match;
    std::string projectName;

    while (std::getline(cmakeFile, line)) {
      if (std::regex_search(line, match, pattern)) {
        projectName = match[1];
        break;
      }
    }

    if (projectName.empty()) {
      std::cerr << RED << "Could not determine project name from CMakeLists.txt." << RESET << std::endl;
      return;
    }
    fs::remove_all(fs::path("./" + projectName));
  }

  const std::string command =
    "cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=" + buildType + " -Wdev -Wdeprecated --log-level=WARNING";
  std::system(command.c_str());

  if (buildType == "release")
    std::system("cmake --build cmake-build-debug --target clean");
  std::system("cmake --build cmake-build-debug");
}
