//
// ProjectLoader.hpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#ifndef PROJECT_LOADER_HPP
#define PROJECT_LOADER_HPP

#include <string>

#include "GamePlugin.hpp"

namespace Editor {
  class ProjectLoader final {
    public:
      /// Build's and load's a project as a shared library
      static bool LoadProject(const std::string &projectPath);

      /// Unload's the currently loaded project
      static void UnloadProject();

      /// @returns The currently loaded plugin, nullptr if non have been loaded
      static IGamePlugin *GetPlugin() {
        return plugin;
      }

      /// @returns True if a project is currently loaded, False if not
      static bool IsProjectLoaded() {
        return plugin != nullptr;
      }

      /// @returns The path to the currently loaded project
      static const std::string &GetProjectPath() {
        return currentProjectPath;
      }
    private:
      /// The path to the currently loaded project
      inline static std::string currentProjectPath;
      /// The library handle of the loaded project
      inline static void *libraryHandle;
      /// The currently loaded plugin
      inline static IGamePlugin *plugin;
      /// The creation function of the current plugin
      inline static CreatePluginFunc createFunc;
      /// The destroy function of the current plugin
      inline static DestroyPluginFunc destroyFunc;

      /// Build's the project as a shared library
      static bool buildProjectAsLibrary(const std::string &projectPath);

      /// Load's the shared library
      static bool loadLibrary(const std::string &libraryPath);

      /// Generate's modified CMakeLists.txt for shared library build
      static bool generateSharedLibraryCMake(const std::string &projectPath, const std::string &tempDir);
  };
}

#endif // PROJECT_LOADER_HPP
