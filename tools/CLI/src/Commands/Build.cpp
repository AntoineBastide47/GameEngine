//
// Build.cpp
// Author: Antoine Bastide
// Date: 10.08.2025
//

#include "Commands/Build.hpp"

#include "Commands/BuildSdk.hpp"

Build::Build()
  : Command(
    "build's the engine static and shared libraries, the sdk and the engine editor", "{|true}", 1, "", true,
    "The release static library can be omitted when building to save time, add the argument 'true' to build it."
  ) {}

void Build::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  commands[COMMAND_BUILD_ENGINE]->Run(args, commands);
  const auto sdkCmd = static_cast<BuildSDK *>(commands[COMMAND_BUILD_SDK].get());
  sdkCmd->DisableLogging();
  sdkCmd->Run({}, commands);
  commands[COMMAND_BUILD_EDITOR]->Run(args, commands);
  sdkCmd->EnableLogging();
  sdkCmd->Run({}, commands);
}
