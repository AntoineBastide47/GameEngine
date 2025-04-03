//
// Rebuild.cpp
// Author: Antoine Bastide
// Date: 03/04/2025
//

#include "commands/Rebuild.hpp"

Rebuild::Rebuild() : Command(std::string("Rebuilds the ") + CLI_COMMAND_NAME + " executable"){}

void Rebuild::Run(const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands) {
  if (constexpr std::string dir = "./CLI"; fs::exists(dir) && fs::is_directory(dir)) {
    std::system("cd CLI && cmake -B build && cmake --build build && cd ..");
  }
}
