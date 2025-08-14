//
// BuildSdk.cpp
// Author: Antoine Bastide
// Date: 01.08.2025
//

#include "Commands/BuildSdk.hpp"

#define SDK_FOLDER_NAME "SDK"
#define SDK_FOLDER "./" SDK_FOLDER_NAME

BuildSDK::BuildSDK()
  : Command("build's the engine's sdk required for releases in the folder " SDK_FOLDER), loggingDisabled(false) {}

void BuildSDK::Run(
  const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  namespace fs = std::filesystem;

  auto safe_copy = [&](const std::string &src, const std::string &dst, const bool is_directory = false) {
    try {
      if (!fs::exists(src)) {
        if (!loggingDisabled)
          std::cerr << "Warning: " << src << " not found, skipping..." << '\n';
        return false;
      }

      if (!loggingDisabled)
        std::cout << "Copying " << src << " to " << dst << "..." << '\n';

      if (is_directory) {
        fs::copy(src, dst, fs::copy_options::recursive);
      } else {
        fs::copy_file(src, dst);
      }
      return true;
    } catch (const fs::filesystem_error &e) {
      if (!loggingDisabled)
        std::cerr << "Failed to copy " << src << ": " << e.what() << '\n';
      return false;
    }
  };

  try {
    const fs::path sdk_folder = SDK_FOLDER;

    // Clean and create SDK folder
    fs::remove_all(sdk_folder);
    fs::create_directories(sdk_folder);

    if (!fs::exists(SDK_FOLDER "/Editor"))
      fs::create_directories(SDK_FOLDER "/Editor");
    if (!fs::exists(SDK_FOLDER "/Editor/Editor"))
      fs::create_directories(SDK_FOLDER "/Editor/Editor");

    if (!loggingDisabled)
      std::cout << "Building the SDK:\n";

    // Copy everything
    safe_copy("./Engine/include", SDK_FOLDER "/Engine", true);
    safe_copy("./Engine/lib", SDK_FOLDER "/lib", true);
    safe_copy("./vendor", SDK_FOLDER "/vendor", true);
    safe_copy("./Templates", SDK_FOLDER "/Templates", true);
    safe_copy("./Editor/include", SDK_FOLDER "/Editor/Editor", true);
    safe_copy("./Editor/EditorExe", SDK_FOLDER "/EditorExe");
    safe_copy("./engine-cli", SDK_FOLDER "/engine-cli");
    safe_copy("./header-forge", SDK_FOLDER "/header-forge");
    safe_copy("./Engine/CMake", SDK_FOLDER "/CMake", true);

    if (!loggingDisabled)
      std::cout << "SDK build completed!" << '\n';
  } catch (const fs::filesystem_error &e) {
    if (!loggingDisabled)
      std::cerr << "SDK build failed: " << e.what() << '\n';
    throw;
  }
}

void BuildSDK::DisableLogging() {
  loggingDisabled = true;
}

void BuildSDK::EnableLogging() {
  loggingDisabled = false;
}
