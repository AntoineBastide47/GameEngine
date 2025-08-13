//
// ProjectLoader.cpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "ProjectLoader.hpp"

namespace Editor {
  ProjectLoader::~ProjectLoader() {
    UnloadProject();
  }

  bool ProjectLoader::LoadProject(const std::string &projectPath) {
    // Unload any existing project
    UnloadProject();

    // Validate project structure
    if (!std::filesystem::exists(projectPath)) {
      std::cerr << "Project path does not exist: " << projectPath << std::endl;
      return false;
    }

    const std::filesystem::path cmakeFile = std::filesystem::path(projectPath) / "CMakeLists.txt";
    const std::filesystem::path gameDir = std::filesystem::path(projectPath) / "Game";

    if (!std::filesystem::exists(cmakeFile) || !std::filesystem::exists(gameDir)) {
      std::cerr << "Invalid project structure. Expected CMakeLists.txt and Game/ directory" << std::endl;
      return false;
    }

    // Build the project as a shared library
    if (!buildProjectAsLibrary(projectPath)) {
      std::cerr << "Failed to build project as shared library" << std::endl;
      return false;
    }

    // Load the shared library (moved to project root by post-build command)
    const std::string libraryPath = projectPath + "/lib" +
                                    std::filesystem::path(projectPath).filename().string() + ".dylib";

    if (!loadLibrary(libraryPath)) {
      std::cerr << "Failed to load shared library: " << libraryPath << std::endl;
      return false;
    }

    currentProjectPath = projectPath;
    return true;
  }

  void ProjectLoader::UnloadProject() {
    if (plugin && destroyFunc) {
      destroyFunc(plugin);
      plugin = nullptr;
    }

    if (libraryHandle) {
      dlclose(libraryHandle);
      libraryHandle = nullptr;
    }

    createFunc = nullptr;
    destroyFunc = nullptr;
    currentProjectPath.clear();
  }

  bool ProjectLoader::buildProjectAsLibrary(const std::string &projectPath) {
    // Create temporary directory for modified CMakeLists.txt
    std::system(("cd " + projectPath + " && mkdir -p editor_debug").c_str());
    const std::string tempDir = projectPath + "/editor_debug";

    // Generate modified CMakeLists.txt for shared library
    if (!generateSharedLibraryCMake(projectPath, tempDir)) {
      return false;
    }

    // Copy the modified CMakeLists.txt to the project root, backing up the original
    std::system(("cp \"" + projectPath + "/CMakeLists.txt\" \"" + projectPath + "/CMakeLists.txt.bak\"").c_str());
    std::system(("cp \"" + tempDir + "/CMakeLists.txt\" \"" + projectPath + "/CMakeLists.txt\"").c_str());

    // Create build directory and build the game
    const std::string buildDir = projectPath + "/build_editor";
    std::filesystem::create_directories(buildDir);
    std::system(("cd \"" + projectPath + "\" && ./engine-cli --build-project-editor debug").c_str());

    // Restore original CMakeLists.txt
    std::system(("cp \"" + projectPath + "/CMakeLists.txt.bak\" \"" + projectPath + "/CMakeLists.txt\"").c_str());
    std::system(("rm \"" + projectPath + "/CMakeLists.txt.bak\"").c_str());

    // Clean up temp directory
    std::filesystem::remove_all(tempDir);

    // Check if library was created
    const std::string libraryPath = projectPath + "/lib" + std::filesystem::path(projectPath).filename().string() +
                                    ".dylib";
    return std::filesystem::exists(libraryPath);
  }

  bool ProjectLoader::loadLibrary(const std::string &libraryPath) {
    // Load the dynamic library
    libraryHandle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!libraryHandle) {
      std::cerr << "Cannot load library: " << dlerror() << std::endl;
      return false;
    }

    // Get the create function
    createFunc = reinterpret_cast<CreatePluginFunc>(dlsym(libraryHandle, "CreatePlugin"));
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      std::cerr << "Cannot load CreatePlugin function: " << dlsym_error << std::endl;
      dlclose(libraryHandle);
      libraryHandle = nullptr;
      return false;
    }

    // Get the destroy function
    destroyFunc = reinterpret_cast<DestroyPluginFunc>(dlsym(libraryHandle, "DestroyPlugin"));
    dlsym_error = dlerror();
    if (dlsym_error) {
      std::cerr << "Cannot load DestroyPlugin function: " << dlsym_error << std::endl;
      dlclose(libraryHandle);
      libraryHandle = nullptr;
      createFunc = nullptr;
      return false;
    }

    // Create plugin instance
    plugin = createFunc();
    if (!plugin) {
      std::cerr << "Failed to create plugin instance" << std::endl;
      dlclose(libraryHandle);
      libraryHandle = nullptr;
      createFunc = nullptr;
      destroyFunc = nullptr;
      return false;
    }

    return true;
  }

  bool ProjectLoader::generateSharedLibraryCMake(const std::string &projectPath, const std::string &tempDir) {
    // Read the original CMakeLists.txt
    const std::string originalCMakeFile = projectPath + "/CMakeLists.txt";
    std::ifstream inFile(originalCMakeFile);
    if (!inFile) {
      std::cerr << "Cannot read CMakeLists.txt: " << originalCMakeFile << std::endl;
      return false;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    // Extract project name
    const std::string projectName = std::filesystem::path(projectPath).filename().string();

    // Modify the CMake content for shared library build
    // Replace add_executable with add_library(SHARED)
    size_t executablePos = content.find("add_executable(${PROJECT_NAME}");
    if (executablePos != std::string::npos) {
      size_t lineEnd = content.find(')', executablePos);
      if (lineEnd != std::string::npos) {
        // Replace add_executable with add_library SHARED and remove main.cpp
        std::string executableSection = content.substr(executablePos, lineEnd - executablePos + 1);
        std::string newSection = executableSection;

        // Replace add_executable with add_library
        newSection = std::regex_replace(newSection, std::regex("add_executable"), "add_library");
        newSection = std::regex_replace(newSection, std::regex("\\$\\{PROJECT_NAME\\}"), "${PROJECT_NAME} SHARED");

        // Remove main.cpp from the sources
        newSection = std::regex_replace(newSection, std::regex("\\s+main\\.cpp"), "");

        content.replace(executablePos, lineEnd - executablePos + 1, newSection);
      }
    }

    // Add plugin export definitions
    const std::string pluginExport = R"(
# Plugin export settings
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

# Export plugin symbols
target_compile_definitions(${PROJECT_NAME} PRIVATE BUILDING_SHARED_LIBRARY)
)";

    content += pluginExport;

    // Write modified CMakeLists.txt to temp directory
    const std::string tempCMakeFile = tempDir + "/CMakeLists.txt";
    std::ofstream outFile(tempCMakeFile);
    if (!outFile) {
      std::cerr << "Cannot write temporary CMakeLists.txt: " << tempCMakeFile << std::endl;
      return false;
    }

    outFile << content;
    outFile.close();

    // Copy any additional files that might be needed
    // This is a simplified approach - you might need to copy more files depending on the project structure

    return true;
  }
}
