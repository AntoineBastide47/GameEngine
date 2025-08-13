//
// RunProject.cpp
// Author: Antoine Bastide
// Date: 31/03/2025
//

#include "Commands/RunProject.hpp"

RunProject::RunProject() : Command("launches the executable of the current project") {}

void RunProject::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
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

  const std::string command = "./" + projectName;
  if (std::system(command.c_str()) != 0) {
    std::cerr << RED << "Failed to run executable: " << command << RESET << std::endl;
  }
}
