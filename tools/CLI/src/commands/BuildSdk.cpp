//
// BuildSdk.cpp
// Author: Antoine Bastide
// Date: 01.08.2025
//

#include "commands/BuildSdk.hpp"

#define SDK_FOLDER_NAME "SDK"
#define SDK_FOLDER "./" SDK_FOLDER_NAME

BuildSDK::BuildSDK()
  : Command("build's the engine's sdk required for releases in the folder " SDK_FOLDER) {}

void BuildSDK::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  std::filesystem::remove_all(SDK_FOLDER);
  std::system("mkdir -p " SDK_FOLDER);
  std::system("cp -r ./include " SDK_FOLDER);
  std::system("cp -r ./vendor " SDK_FOLDER);
  std::system("mv " SDK_FOLDER "/include/Engine2D-*.a " SDK_FOLDER);
}
