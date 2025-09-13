//
// Build.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include <string>

#include "Commands/BuildEngine.hpp"

#define ARGS " -Wdev -Wdeprecated --log-level=WARNING "
#define BUILD_ENGINE "-DBUILD_ENGINE=ON -DBUILD_EDITOR=OFF"

#define ENGINE_INCLUDE "Engine/include"
#define ENGINE_BUILD "Engine/build"

#define DEBUG_CMD " -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=0" ARGS BUILD_ENGINE
#define PROFILING_CMD " -S . -DCMAKE_BUILD_TYPE=Debug -DPROFILE=1" ARGS BUILD_ENGINE
#define RELEASE_CMD " -S . -DCMAKE_BUILD_TYPE=Release -DPROFILE=0 -DBUILD_ENGINE_SHARED=0" ARGS BUILD_ENGINE

BuildEngine::BuildEngine()
  : Command(
    "builds the engine's static and shared libraries.", "{|true}", 1, "", true,
    "The release static library can be omitted when building to save time, add the argument 'true' to build it."
  ) {}

void BuildEngine::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  const std::string &buildRelease = args[0];

  if (!args.empty() && buildRelease != "true") {
    std::cout << RED << R"(Unexpected input, expected: "" or "true", got: )" << buildRelease << "\n" << "Try: "
        << CLI_EXECUTE_COMMAND(COMMAND_HELP) << " for more information." << RESET;
    return;
  }

  if (!fs::exists("Engine/build"))
    fs::create_directory("Engine/build");
  if (!fs::exists("Engine/lib"))
    fs::create_directory("Engine/lib");
  if (!fs::exists("Engine/lib/static"))
    fs::create_directory("Engine/lib/static");
  if (!fs::exists("Engine/lib/shared"))
    fs::create_directory("Engine/lib/shared");

  // Remove previously built libraries
  fs::remove_all("Engine/lib/shared");
  fs::create_directory("Engine/lib/shared");
  fs::remove_all("Engine/lib/static");
  fs::create_directory("Engine/lib/static");

  const bool hasNinja = HasNinja();

  // Header Forge
  std::system(
    "./header-forge --parse ./" ENGINE_INCLUDE " --compilerArgs -I./" ENGINE_INCLUDE
    " -I./vendor/glfw/include -I./vendor/glm -I./vendor/cpptrace/include -I./" ENGINE_BUILD
    "/debug_shared/_cmrc/include -I./vendor/imgui "
  );

  for (const std::vector<std::string> buildTypes = {"static", "shared"}; const auto &type: buildTypes) {
    const std::string isShared = std::to_string(type == "shared");
    const std::string buildFolder = "_" + type;
    const std::string buildShared = " -DBUILD_ENGINE_SHARED=" + isShared;
    const std::string editorMacros = " -D_e_WITH_EDITOR=" + isShared;

    // Debug
    const std::string debugDir = ENGINE_BUILD "/debug" + buildFolder;
    if (!fs::exists(debugDir))
      fs::create_directory(debugDir);

    std::system(
      (hasNinja
         ? "cmake -G Ninja -B " + debugDir + DEBUG_CMD + buildShared + editorMacros
         : "cmake -B " + debugDir + DEBUG_CMD + buildShared + editorMacros
      ).c_str()
    );
    std::system(("cmake --build " + debugDir + " --parallel").c_str());

    // Profiling
    const std::string profilingDir = ENGINE_BUILD "/profiling" + buildFolder;
    if (!fs::exists(profilingDir))
      fs::create_directory(profilingDir);

    std::system(
      (hasNinja
         ? "cmake -G Ninja -B " + profilingDir + PROFILING_CMD + buildShared + editorMacros
         : "cmake -B " + profilingDir + PROFILING_CMD + buildShared + editorMacros
      ).c_str()
    );
    std::system(("cmake --build " + profilingDir + " --parallel").c_str());
  }

  // Release
  if (buildRelease == "true") {
    fs::remove_all(ENGINE_BUILD "/release");
    fs::create_directory(ENGINE_BUILD "/release");

    const std::string releaseDir = ENGINE_BUILD "/release";
    if (!fs::exists(releaseDir))
      fs::create_directory(releaseDir);

    std::system(
      (hasNinja
         ? "cmake -G Ninja -B " + releaseDir + RELEASE_CMD
         : "cmake -B " + releaseDir + RELEASE_CMD
      ).c_str()
    );
    std::system(("cmake --build " + releaseDir + " --parallel").c_str());
  }
}
