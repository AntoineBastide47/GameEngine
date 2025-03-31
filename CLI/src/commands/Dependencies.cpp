//
// Dependencies.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "commands/Dependencies.hpp"

Dependencies::Dependencies() : Command("Fetches and Update the GitHub submodules linked to this project") {}

constexpr std::array<std::pair<const char *, const char *>, 3> submodules = {{
  {"glfw", "https://github.com/glfw/glfw.git"},
  {"glm", "https://github.com/g-truc/glm.git"},
  {"cpptrace", "https://github.com/jeremy-rifkin/cpptrace.git"},
}};

void Dependencies::Run(const std::vector<std::string> &, const OrderedMap<std::string, std::unique_ptr<Command>> &) {
  for (const auto &[name, url]: submodules) {
    constexpr auto dir = "./vendor";
    char path[std::strlen(dir) + 1 + strlen(name) + 1];
    std::snprintf(path, sizeof(path), "%s/%s", dir, name);
    if (std::filesystem::path p(path); !exists(p) || !is_directory(p)) {
      constexpr auto commandStart = "git submodule add --force";
      char command[std::strlen(commandStart) + 1 + strlen(url) + 1 + strlen(path) + 1];
      std::snprintf(command, sizeof(command), "%s %s %s", commandStart, url, path);
      std::system(command);
    }
  }

  std::system("git submodule update --init --recursive");
}
