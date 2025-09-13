//
// Command.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <fstream>
#include <filesystem>
#include <string>
#include <regex>
#include <iostream>

#include "OrderedMap.hpp"

namespace fs = std::filesystem;

constexpr auto BOLD = "\033[1m";
constexpr auto RESET = "\033[0m";
constexpr auto RED = "\033[31m";
constexpr auto GREEN = "\033[32m";

static constexpr std::string CLI_VERSION = "1.0.0";
static constexpr std::string CLI_COMMAND_NAME = "engine-cli";

static constexpr std::string COMMAND_HELP = "help";
static constexpr std::string COMMAND_VERSION = "version";

static constexpr std::string COMMAND_REBUILD = "rebuild";
static constexpr std::string COMMAND_BUILD = "build";
static constexpr std::string COMMAND_BUILD_TOOLS = "build-tools";
static constexpr std::string COMMAND_BUILD_ENGINE = "build-engine";
static constexpr std::string COMMAND_BUILD_SDK = "build-sdk";
static constexpr std::string COMMAND_BUILD_EDITOR = "build-editor";
static constexpr std::string COMMAND_CREATE_PROJECT = "create-project";
static constexpr std::string COMMAND_LINK_PROJECT = "link-project";
static constexpr std::string COMMAND_DEPENDENCIES = "dependencies";

static constexpr std::string COMMAND_BUILD_PROJECT = "build-project";
static constexpr std::string COMMAND_BUILD_PROJECT_EDITOR = "build-project-editor";
static constexpr std::string COMMAND_RUN_PROJECT = "run";

static inline std::string CLI_COMMAND() {
  return "./" + CLI_COMMAND_NAME;
}

static inline std::string CLI_EXECUTE_COMMAND_NO_QUOTES(const std::string &name) {
  return CLI_COMMAND() + " --" + name;
}

static inline std::string CLI_EXECUTE_COMMAND(const std::string &name) {
  return "'" + CLI_EXECUTE_COMMAND_NO_QUOTES(name) + "'";
}

namespace fs = std::filesystem;

class Command {
  friend int main(int argc, const char *argv[]);
  public:
    const std::string description;
    const std::string args;
    const std::string defaultArg;
    const std::string note;
    const int argCount;
    const bool argsAreOptional;
    const bool hidden;

    explicit Command(
      std::string description, std::string args = "", const int argCount = 0, std::string defaultArg = "",
      const bool argsAreOptional = false, std::string note = "", bool hidden = false
    )
      : description(std::move(description)), args(std::move(args)), defaultArg(std::move(defaultArg)),
        note(std::move(note)), argCount(argCount), argsAreOptional(argsAreOptional), hidden(hidden) {}

    virtual ~Command() = default;
    virtual void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) = 0;

    static bool HasNinja();
  private:
    void run(std::vector<std::string> args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands);
};

#endif //COMMAND_HPP
