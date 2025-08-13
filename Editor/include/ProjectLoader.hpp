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
      ProjectLoader() = default;
      ~ProjectLoader();

      /// Build's and load's a project as a shared library
      bool LoadProject(const std::string &projectPath);

      /// Unload's the currently loaded project
      void UnloadProject();

      /// @returns The currently loaded plugin, nullptr if non have been loaded
      IGamePlugin *GetPlugin() const {
        return plugin;
      }

      /// @returns True if a project is currently loaded, False if not
      bool IsProjectLoaded() const {
        return plugin != nullptr;
      }

      /// @returns The path to the currently loaded project
      const std::string &GetProjectPath() const {
        return currentProjectPath;
      }
    private:
      /// The path to the currently loaded project
      std::string currentProjectPath;
      /// The library handle of the loaded project
      void *libraryHandle;
      /// The currently loaded plugin
      IGamePlugin *plugin;
      /// The creation function of the current plugin
      CreatePluginFunc createFunc;
      /// The destroy function of the current plugin
      DestroyPluginFunc destroyFunc;

      /// Build's the project as a shared library
      static bool buildProjectAsLibrary(const std::string &projectPath);

      /// Load's the shared library
      bool loadLibrary(const std::string &libraryPath);

      /// Generate's modified CMakeLists.txt for shared library build
      static bool generateSharedLibraryCMake(const std::string &projectPath, const std::string &tempDir);
  };
}

#endif // PROJECT_LOADER_HPP
