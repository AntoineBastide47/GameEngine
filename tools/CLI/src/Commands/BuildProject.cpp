//
// BuildProject.cpp
// Author: Antoine Bastide
// Date: 31/03/2025
//

#include "Commands/BuildProject.hpp"
#include "Commands/BuildEngine.hpp"

BuildProject::BuildProject(const bool fromEditor)
  : Command(
      "build's the project's executable in debug or release mode", "{debug|profile|release}", 1, "debug", false, "",
      fromEditor
    ),
    fromEditor(fromEditor) {}

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
    std::cerr << RED << "CMakeLists.txt not found in the current directory." << RESET;
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
    std::cerr << RED << "Could not determine project name from CMakeLists.txt." << RESET;
    return;
  }
  fs::remove_all(fs::path("./" + projectName));

  const std::string prefix = "./lib" + projectName + ".";
  try {
    for (const auto &entry: fs::directory_iterator("./")) {
      if (entry.is_regular_file()) {
        if (const std::string filename = entry.path().filename().string();
          filename.starts_with("lib" + projectName + ".")) {
          fs::remove(entry.path());
          std::cout << "Removed: " << filename << std::endl;
        }
      }
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error removing files: " << e.what() << std::endl;
  }

  // Header Forge
  const std::string engineInclude = "-I" + enginePath + "/Engine ";
  const std::string vendorInclude =
      "-I" + enginePath + "/vendor/glfw/include "
      "-I" + enginePath + "/vendor/glm "
      "-I" + enginePath + "/vendor/cpptrace/include "
      "-I" + enginePath + "/build/debug/_cmrc/include ";
  //std::system(
  //  ("./header-forge --parse ./Game/include --compilerArgs -I./Game/include " + engineInclude + vendorInclude).c_str()
  //);

  const std::string parallel = " --parallel " + std::to_string(GetCoreCount());
  const std::string buildDir = fromEditor ? "build_editor" : "build";
  const std::string buildType = fromEditor ? " -DPROJECT_BUILD_TYPE=shared" : " -DPROJECT_BUILD_TYPE=static";

  std::system(
    HasNinja()
      ? (
        "cmake -G Ninja -B " + buildDir + " -S . -DBUILD_TYPE=" + arg + " -Wdev -Wdeprecated --log-level=WARNING" +
        buildType
      ).c_str()
      : (
        "cmake -B " + buildDir + " -S . -DBUILD_TYPE=" + arg + " -Wdev -Wdeprecated --log-level=WARNING" + buildType
      ).c_str()
  );
  std::system(("cmake --build " + buildDir + parallel).c_str());
}
