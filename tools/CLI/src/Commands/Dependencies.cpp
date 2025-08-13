//
// Dependencies.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "Commands/Dependencies.hpp"

Dependencies::Dependencies()
  : Command("Fetches and Upgrades the GitHub submodules linked to this project") {}

constexpr std::array<std::array<const char *, 3>, 4> submodules = {
  {
    {"glfw", "https://github.com/glfw/glfw.git", "master"},
    {"glm", "https://github.com/g-truc/glm.git", "master"},
    {"cpptrace", "https://github.com/jeremy-rifkin/cpptrace.git", "main"},
    {"imgui", "https://github.com/ocornut/imgui.git", "docking"}
  }
};

int runCommand(const std::string &command) {
  std::cout << "Running: " << command << std::endl;
  const int result = std::system(command.c_str());
  if (result != 0)
    std::cerr << "Command failed with code " << result << ": " << command << std::endl;
  return result;
}

void Dependencies::Run(const std::vector<std::string> &, const OrderedMap<std::string, std::unique_ptr<Command>> &) {
  const std::string dir = "./vendor";

  // Ensure vendor directory exists
  std::filesystem::create_directories(dir);

  std::cout << "Updating submodules recursively..." << std::endl;
  if (std::system("git submodule update --init --recursive") != 0) {
    std::cerr << "Failed to update submodules recursively" << std::endl;
  }

  for (const auto &[name, url, branch]: submodules) {
    const std::string path = dir + "/" + name;

    if (std::filesystem::path p(path); !exists(p) || !is_directory(p)) {
      // Use string concatenation instead of manual buffer management
      const std::string command = "git submodule add --force " + std::string(url) + " " + path;
      std::cout << "Adding submodule: " << command << std::endl;

      if (std::system(command.c_str()) != 0) {
        std::cerr << "Failed to add submodule: " << name << std::endl;
        continue;
      }
    }

    // Separate the commands for better error handling
    const std::string checkout_cmd = "cd " + path + " && git checkout " + std::string(branch);
    const std::string pull_cmd = "cd " + path + " && git pull origin " + std::string(branch);

    std::cout << "Checking out branch " << branch << " for " << name << std::endl;
    if (std::system(checkout_cmd.c_str()) != 0) {
      std::cerr << "Failed to checkout branch " << branch << " for " << name << std::endl;
    }

    std::cout << "Pulling latest changes for " << name << std::endl;
    if (std::system(pull_cmd.c_str()) != 0) {
      std::cerr << "Failed to pull changes for " << name << std::endl;
    }
  }
}
