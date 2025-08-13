//
// GamePlugin.hpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#ifndef GAME_PLUGIN_HPP
#define GAME_PLUGIN_HPP

#include <functional>
#include <memory>
#include <string>
#include <cmrc/cmrc.hpp>

namespace Engine2D {
  class Game2D;
}

namespace Editor {
  using ResourceLoader = std::function<cmrc::file(const std::string &)>;

  class IGamePlugin {
    public:
      virtual ~IGamePlugin() = default;

      /// Create's a game instance in headless mode
      virtual std::unique_ptr<Engine2D::Game2D> CreateGame(int width, int height) = 0;

      /// Set's up resource loader for the game
      virtual void SetupResources(Engine2D::Game2D *game) = 0;

      /// @returns The name of this game
      virtual std::string GetName() const = 0;
      /// @returns The version of this game
      virtual std::string GetVersion() const = 0;
  };

  /// Plugin creation function signature
  typedef IGamePlugin * (*CreatePluginFunc)();
  /// Plugin destruction function signature
  typedef void (*DestroyPluginFunc)(IGamePlugin *);
}

// Macro to export plugin functions
#define EXPORT_GAME_PLUGIN(PluginClass) \
  extern "C" { \
    Editor::IGamePlugin* CreatePlugin() { \
      return new PluginClass(); \
    } \
    \
    void DestroyPlugin(Editor::IGamePlugin* plugin) { \
      delete plugin; \
    } \
  }

#endif // GAME_PLUGIN_HPP
