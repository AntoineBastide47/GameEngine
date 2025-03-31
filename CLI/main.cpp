#include <iostream>
#include <map>

#include "Command.hpp"
#include "OrderedMap.hpp"
#include "commands/Build.hpp"
#include "commands/BuildProject.hpp"
#include "commands/CreateProject.hpp"
#include "commands/Dependencies.hpp"
#include "commands/Help.hpp"
#include "commands/LinkProject.hpp"
#include "commands/RunProject.hpp"
#include "commands/Version.hpp"

static OrderedMap<std::string, std::unique_ptr<Command>> CreateEngineCommands() {
  OrderedMap<std::string, std::unique_ptr<Command>> cmds;
  cmds.insert(COMMAND_HELP, std::make_unique<Help>());
  cmds.insert(COMMAND_DEPENDENCIES, std::make_unique<Dependencies>());
  cmds.insert(COMMAND_BUILD, std::make_unique<Build>());
  cmds.insert(COMMAND_CREATE_PROJECT, std::make_unique<CreateProject>());
  cmds.insert(COMMAND_LINK_PROJECT, std::make_unique<LinkProject>());
  cmds.insert(COMMAND_VERSION, std::make_unique<Version>());
  return cmds;
}

static OrderedMap<std::string, std::unique_ptr<Command>> CreateProjectCommands() {
  OrderedMap<std::string, std::unique_ptr<Command>> cmds;
  cmds.insert(COMMAND_HELP, std::make_unique<Help>());
  cmds.insert(COMMAND_BUILD_PROJECT, std::make_unique<BuildProject>());
  cmds.insert(COMMAND_RUN_PROJECT, std::make_unique<RunProject>());
  cmds.insert(COMMAND_VERSION, std::make_unique<Version>());
  return cmds;
}

// TODO: add a file in GameEngine with expected version of cli and if different auto rebuild
int main(const int argc, const char *argv[]) {
  #if !defined(_WIN32) && !(defined(__APPLE__) && defined(__MACH__)) && !defined(__linux__)
    #error "Unsupported platform. The engine only supports MacOS, Linux and Windows for now."
  #endif

  bool useEngineCommands = false;
  std::ifstream cmake("CMakeLists.txt");
  if (std::string firstLine; cmake && std::getline(cmake, firstLine)) {
    useEngineCommands = (firstLine == "## Engine CMake <-- do not remove line");
  }

  const auto commands = useEngineCommands ? CreateEngineCommands() : CreateProjectCommands();

  if (argc < 2) {
    commands[COMMAND_HELP]->run({""}, commands);
    return 0;
  }

  std::vector<std::string> args(argv, argv + argc);
  args.erase(args.begin());
  if (commands.contains(args[0])) {
    commands[args[0]]->run(args, commands);
    return 1;
  }

  std::cout << RED << "Command " << args[0] << " not found." << "\n";
  std::cout << "Try '" << CLI_COMMAND << " help' for available commands." << RESET << std::endl;
  return 0;
}
