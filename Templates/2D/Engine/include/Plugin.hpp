//
// BrickBreakerPlugin.hpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "Editor/GamePlugin.hpp"

// Forward declarations
namespace Engine2D {
  class Game2D;
}

class {{PROJECT_NAME}};

class Plugin final : public Editor::IGamePlugin {
  public:
    Plugin() = default;
    ~Plugin() override = default;

    // IGamePlugin implementation
    std::unique_ptr<Engine2D::Game2D> CreateGame(int width, int height) override;
    void SetupResources(Engine2D::Game2D *game) override;

    [[nodiscard]] std::string GetName() const override {
      return "BrickBreaker";
    }

    [[nodiscard]] std::string GetVersion() const override {
      return "1.0.0";
    }
};

// Export plugin functions
EXPORT_GAME_PLUGIN(Plugin)

#endif // PLUGIN_HPP
