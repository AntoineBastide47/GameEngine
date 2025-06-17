//
// BuildTools.cpp
// Author: Antoine Bastide
// Date: 17.06.2025
//

#include "commands/BuildTools.hpp"

BuildTools::BuildTools()
  : Command("build's the engine tools", "", 0, "", false, "Does not rebuild the " + CLI_COMMAND_NAME + " executable.") {}

void BuildTools::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  const std::filesystem::path path("./tools");
  if (!std::filesystem::exists(path)) {
    std::cout << RED << "Error: folder ./tools not found" << RESET << "\n";
    return;
  }

  for (const auto &file: std::filesystem::directory_iterator(path)) {
    if (std::filesystem::is_directory(file) && file.path().filename().string() != "CLI") {
      std::system(("cd " + file.path().string() + " && cmake -B build && cmake --build build").c_str());
    }
  }
}
