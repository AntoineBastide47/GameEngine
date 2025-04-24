//
// Help.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "commands/Help.hpp"

Help::Help() : Command("displays information related to the given command", "<command-name>", 1, "version", true) {}

void Help::Run(const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands) {
  if (args.size() == 1) {
    if (commands.contains(args[0])) {
      displayCommandInfo(args[0], commands[args[0]]);
    } else {
      //TODO: add: ", did you mean 'closest command name to args[0]'
      std::cout << RED << "Command " << args[0] << " not found." << "\n";
      std::cout << "Try " << CLI_EXECUTE_COMMAND(COMMAND_HELP) << " for available commands." << std::endl;
    }
    return;
  }

  std::cout << "engine-cli is a tool to help you build the engine, create and link projects all from your terminal";
  std::cout << "\n\nAvailable commands:";
  for (const auto &[name, command]: commands) {
    std::cout << "\n";
    displayCommandInfo(name, command);
  }
}

void Help::displayCommandInfo(const std::string &name, const std::unique_ptr<Command> &command) {
  std::cout << BOLD << name << RESET << ": " << command->description << "\n";
  if (!command->note.empty())
    std::cout << "    Note:   " << command->note << "\n";
  std::cout << "    Usage:   " << CLI_EXECUTE_COMMAND_NO_QUOTES(name) << " " << command->args;
  if (!command->defaultArg.empty())
    std::cout << "\n    Example: " << CLI_EXECUTE_COMMAND_NO_QUOTES(name) << " " << command->defaultArg;
}
