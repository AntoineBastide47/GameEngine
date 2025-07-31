//
// BuildProject.cpp
// Author: Antoine Bastide
// Date: 31/03/2025
//

#include "commands/BuildProject.hpp"
#include "commands/Build.hpp"

BuildProject::BuildProject()
  : Command("builds the project's executable in debug or release mode", "{debug|profile|release}", 1, "debug") {}

void BuildProject::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  const std::string &arg = args[0];
  if (arg != "debug" && arg != "profile" && arg != "release") {
    std::cout << RED << "Unknown build type: " << arg << "\n" << "Try: " << CLI_EXECUTE_COMMAND(COMMAND_BUILD_PROJECT)
        << RESET;
    return;
  }

  std::ifstream cmakeFile("CMakeLists.txt");
  if (!cmakeFile) {
    std::cerr << RED << "CMakeLists.txt not found in the current directory." << RESET << std::endl;
    return;
  }

  std::string line;
  std::regex pattern(R"(^\s*project\s*\(\s*([a-zA-Z0-9_\-]+)\b.*\))", std::regex::icase);
  std::regex pattern2(R"(set\s*\(\s*ENGINE_LOCATION\s+([^\)]+)\s*\))", std::regex::icase);
  std::smatch match;
  std::smatch match2;
  std::string projectName;
  std::string enginePath;

  while (std::getline(cmakeFile, line)) {
    if (std::regex_search(line, match, pattern))
      projectName = match[1];

    if (std::regex_search(line, match2, pattern2))
      enginePath = match2[1];

    if (!projectName.empty() && !enginePath.empty())
      break;
  }

  if (projectName.empty()) {
    std::cerr << RED << "Could not determine project name from CMakeLists.txt." << RESET << std::endl;
    return;
  }
  fs::remove_all(fs::path("./" + projectName));

  // Header Forge
  const std::string engineInclude = "-I" + enginePath + "/include ";
  const std::string vendorInclude =
      "-I" + enginePath + "/vendor/glfw/include "
      "-I" + enginePath + "/vendor/glm "
      "-I" + enginePath + "/vendor/cpptrace/include "
      "-I" + enginePath + "/build/debug/_cmrc/include ";
  std::system(
    ("./header-forge --parse ./Game/include --compilerArgs -I./include " + engineInclude + vendorInclude).c_str()
  );

  const std::string parallel = " --parallel " + std::to_string(Build::GetCoreCount());

  std::system(
    Build::HasNinja()
      ? ("cmake -G Ninja -B build -S . -DBUILD_TYPE=" + arg + " -Wdev -Wdeprecated --log-level=WARNING").c_str()
      : ("cmake -B build -S . -DBUILD_TYPE=" + arg + " -Wdev -Wdeprecated --log-level=WARNING").c_str()
  );
  std::system(("cmake --build build" + parallel).c_str());
}
