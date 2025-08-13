//
// BuildEditor.cpp
// Author: Antoine Bastide
// Date: 10.08.2025
//

#include "Commands/BuildEditor.hpp"

#define ARGS " -Wdev -Wdeprecated --log-level=WARNING "
#define EDITOR_BUILD "Editor/build"
#define BUILD_EDITOR "-DBUILD_ENGINE=OFF -DBUILD_EDITOR=ON"

#define EDITOR_DEBUG_CMD " -S . -DCMAKE_BUILD_TYPE=Debug" ARGS BUILD_EDITOR
#define EDITOR_RELEASE_CMD " -S . -DCMAKE_BUILD_TYPE=Release" ARGS BUILD_EDITOR

BuildEditor::BuildEditor()
  : Command(
    "build's the engine's editor", "{|true}", 1, "", true,
    "The release static library can be omitted when building to save time, add the argument 'true' to build it."
  ) {}

void BuildEditor::Run(
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

  // Remove previously built executable
  fs::remove("Engine/lib/EditorExe");

  const std::string parallel = " --parallel " + std::to_string(GetCoreCount());

  // Release
  if (buildRelease == "true") {
    fs::remove_all(EDITOR_BUILD "/release");
    fs::create_directory(EDITOR_BUILD "/release");
    std::system(
      HasNinja()
        ? "cmake -G Ninja -B " EDITOR_BUILD "/release" EDITOR_RELEASE_CMD
        : "cmake -B " EDITOR_BUILD "/release" EDITOR_RELEASE_CMD
    );
    std::system(("cmake --build " EDITOR_BUILD "/release" + parallel).c_str());
  } else {
    if (!fs::exists("Engine/build/debug"))
      fs::create_directory("Engine/build/debug");

    std::system(
      HasNinja()
        ? "cmake -G Ninja -B " EDITOR_BUILD "/debug" EDITOR_DEBUG_CMD
        : "cmake -B " EDITOR_BUILD "/debug" EDITOR_DEBUG_CMD
    );
    std::system(("cmake --build " EDITOR_BUILD "/debug" + parallel).c_str());
  }
}
