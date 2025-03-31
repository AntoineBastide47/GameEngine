//
// Version.cpp
// Author: Antoine Bastide
// Date: 28/03/2025
//

// ReSharper disable CppDFAConstantConditions

#include "commands/Version.hpp"

constexpr std::array<std::string, 1> expectedLibraries = {
  "Engine2D"
};

Version::Version() : Command("displays the version of the engine currently installed") {}

void Version::Run(const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands) {
  std::array<bool, expectedLibraries.size()> found{};
  std::array<std::string, expectedLibraries.size()> versions{};

  std::ifstream cmakeFile("CMakeLists.txt");
  if (!cmakeFile) {
    std::cerr << RED << "CMakeLists.txt not found in the current directory." << RESET << std::endl;
    return;
  }

  std::string line;
  std::regex pattern(R"(^\s*set\s*\(\s*ENGINE_LOCATION\s+([^)]+)\))", std::regex::icase);
  std::smatch match;
  std::string engineLocation;

  while (std::getline(cmakeFile, line)) {
    if (std::regex_search(line, match, pattern)) {
      engineLocation = match[1];
      break;
    }
  }

  if (engineLocation.empty()) {
    std::cerr << RED << "Could not determine engine location from CMakeLists.txt." << RESET << std::endl;
    return;
  }

  for (const std::string dir = commands.contains(COMMAND_BUILD_PROJECT) ? engineLocation + "/include" : "./include";
    const auto &entry: fs::directory_iterator(dir)) {
    if (entry.path().extension() == ".a") {
      std::string filename = entry.path().stem().string();

      if (const size_t dashIndex = filename.find('-'); dashIndex != std::string::npos) {
        const std::string libraryName = filename.substr(0, dashIndex);
        const std::string libraryVersion = filename.substr(dashIndex + 1);

        if (const auto it = std::ranges::find(expectedLibraries, libraryName); it != expectedLibraries.end()) {
          const size_t index = std::distance(expectedLibraries.begin(), it);
          found[index] = true;
          versions[index] = libraryVersion;
        }
      }
    }
  }

  std::cout << BOLD << "engine-cli" << RESET << " installed (version " << BOLD << CLI_VERSION << RESET << ")\n";
  for (size_t i = 0; i < expectedLibraries.size(); ++i) {
    if (expectedLibraries[i].empty())
      continue;

    if (!found[i])
      std::cout << RED << "Library" << BOLD << expectedLibraries[i] << RESET << RED
        << " was not found in the " << BOLD << "include" << RESET << RED << " folder.\n  "
        << "Please check your engine installation and try again." << RESET;
    else
      std::cout << BOLD << expectedLibraries[i] << RESET << " library installed (version " << BOLD
        << versions[i] << RESET << ")";

    if (i < expectedLibraries.size() - 1)
      std::cout << "\n";
  }
}
