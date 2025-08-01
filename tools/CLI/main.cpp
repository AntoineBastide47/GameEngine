#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "Command.hpp"
#include "OrderedMap.hpp"
#include "Utils.hpp"
#include "commands/Build.hpp"
#include "commands/BuildProject.hpp"
#include "commands/BuildSdk.hpp"
#include "commands/BuildTools.hpp"
#include "commands/CreateProject.hpp"
#include "commands/Dependencies.hpp"
#include "commands/Help.hpp"
#include "commands/LinkProject.hpp"
#include "commands/Rebuild.hpp"
#include "commands/RunProject.hpp"
#include "commands/Version.hpp"

static OrderedMap<std::string, std::unique_ptr<Command>> CreateEngineCommands() {
  OrderedMap<std::string, std::unique_ptr<Command>> cmds;
  cmds.insert(COMMAND_HELP, std::make_unique<Help>());
  cmds.insert(COMMAND_DEPENDENCIES, std::make_unique<Dependencies>());
  cmds.insert(COMMAND_BUILD, std::make_unique<Build>());
  cmds.insert(COMMAND_BUILD_TOOLS, std::make_unique<BuildTools>());
  cmds.insert(COMMAND_CREATE_PROJECT, std::make_unique<CreateProject>());
  cmds.insert(COMMAND_LINK_PROJECT, std::make_unique<LinkProject>());
  cmds.insert(COMMAND_VERSION, std::make_unique<Version>());
  cmds.insert(COMMAND_REBUILD, std::make_unique<Rebuild>());
  cmds.insert(COMMAND_BUILD_SDK, std::make_unique<BuildSDK>());
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

int main(const int argc, const char *argv[]) {
  #if !defined(_WIN32) && !(defined(__APPLE__) && defined(__MACH__)) && !defined(__linux__)
    #error "Unsupported platform. The engine only supports MacOS, Linux and Windows for now."
  #endif

  bool useEngineCommands = false;
  if (std::ifstream cmake("CMakeLists.txt"); cmake) {
    std::string line;
    while (std::getline(cmake, line)) {
      if (line.find("add_library(Engine2D STATIC") != std::string::npos) {
        useEngineCommands = true;
        break;
      }
    }
  }

  const auto commands = useEngineCommands ? CreateEngineCommands() : CreateProjectCommands();

  if (argc < 2) {
    commands[COMMAND_HELP]->run({""}, commands);
    return 0;
  }

  // Validate all command args start with "--"
  if (std::string(argv[1]).rfind("--", 0) != 0) {
    std::cout << RED << "Expected command to start with '--'. Got: '" << argv[1] << "'\n";
    std::cout << "Try " << CLI_EXECUTE_COMMAND(COMMAND_HELP) << " for usage." << RESET << std::endl;
    return 0;
  }

  std::vector<std::string> argvStr(argv, argv + argc);
  argvStr.erase(argvStr.begin());
  std::string result = Utils::Join(argvStr, " ");

  std::vector<std::string> userCommands = Utils::Split(result, "--");
  std::vector<std::vector<std::string>> commandArgs;
  for (auto it = userCommands.begin(); it != userCommands.end();) {
    *it = Utils::Trim(*it);
    if (it->empty())
      it = userCommands.erase(it);
    else {
      commandArgs.emplace_back(Utils::Split(*it, " "));
      ++it;
    }
  }

  for (const auto &args: commandArgs) {
    if (!commands.contains(args[0])) {
      std::cout << RED << "Command " << args[0] << " not found." << "\n";
      std::cout << "Try '" << CLI_COMMAND() << " help' for available commands." << RESET << std::endl;
      return 0;
    }
  }

  for (const auto &args: commandArgs)
    commands[args[0]]->run(args, commands);

  return 1;
}
