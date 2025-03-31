//
// BuildProject.cpp
// Author: Antoine Bastide
// Date: 31/03/2025
//

#include "commands/BuildProject.hpp"

#include "commands/Build.hpp"

BuildProject::BuildProject()
  : Command("builds the project's executable in debug or release mode", "{debug|release}", 1, "debug") {}

void BuildProject::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  auto newArgs = args;
  newArgs.emplace_back("project");
  Build().Run(newArgs, commands);
}
