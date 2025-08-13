//
// Command.cpp
// Author: Antoine Bastide
// Date: 10.08.2025
//

#include <sys/sysctl.h>

#include "Command.hpp"

int Command::GetCoreCount() {
  #if defined(_WIN32)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
  #elif defined(__APPLE__)
  int count;
  size_t size = sizeof(count);
  sysctlbyname("hw.ncpu", &count, &size, nullptr, 0);
  return count;
  #elif defined(__linux__)
  return sysconf(_SC_NPROCESSORS_ONLN);
  #else
  return 2;
  #endif
}

bool Command::HasNinja() {
  #if defined(_WIN32)
  return std::system("where ninja >nul 2>&1") == 0;
  #else
  return std::system("command -v ninja >/dev/null 2>&1") == 0;
  #endif
}

void Command::run(std::vector<std::string> args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands) {
  if (const size_t actualArgs = args.size() - 1;
    (argsAreOptional && actualArgs > argCount) || (!argsAreOptional && actualArgs != argCount)) {
    std::cout << RED << "Invalid number of arguments, expected ";
    if (argsAreOptional)
      std::cout << "at most ";
    std::cout << argCount << " but got " << actualArgs << "\n"
        << "Try '" << CLI_COMMAND() << " help " << args[0] << "' for more information." << RESET << std::endl;
    return;
  }

  args.erase(args.begin());
  Run(args, commands);
  std::cout << std::endl;
}
