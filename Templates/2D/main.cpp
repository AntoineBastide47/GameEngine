#include <iostream>
#include <cmrc/cmrc.hpp>

#include "2D/Game2D.h"
#include "GameCode/{{PROJECT_NAME}}.h"

CMRC_DECLARE(resources);
CMRC_DECLARE(engine_resources);

int main() {
  {{PROJECT_NAME}} game{800, 600};
  game.SetGameResourceLoader([](const std::string& path) {
    auto fs = cmrc::resources::get_filesystem();
    if (!fs.exists(path)) {
      fs = cmrc::engine_resources::get_filesystem();
      if (!fs.exists(path))
        throw std::runtime_error("No such file or directory: " + path);
    }
    return fs.open(path);
  });
  game.Run();
  return 0;
}
