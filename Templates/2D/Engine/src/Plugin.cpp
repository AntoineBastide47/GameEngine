#include "Plugin.hpp"
#include "{{PROJECT_NAME}}.hpp"

// Declare the embedded resources
CMRC_DECLARE(assets);
CMRC_DECLARE(engine_resources);

std::unique_ptr<Engine2D::Game2D> Plugin::CreateGame(int width, int height) {
  auto game = std::make_unique<{{PROJECT_NAME}}>(width, height);
  return std::move(game);
}

void Plugin::SetupResources(Engine2D::Game2D* game) {
  if (!game)
    return;

  // Set up resource loader for the game
  game->SetGameResourceLoader([](const std::string &path) {
    auto fs = cmrc::assets::get_filesystem();
    const std::string updated_path = path.starts_with("Game/") ? path : "Game/" + path;

    if (!fs.exists(updated_path)) {
      fs = cmrc::engine_resources::get_filesystem();
      if (!fs.exists(path))
        throw std::runtime_error("No such file or directory: " + path);
      return fs.open(path);
    }
    return fs.open(updated_path);
  });
}
