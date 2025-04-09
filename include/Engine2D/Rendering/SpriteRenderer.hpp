//
// SpriteRenderer.hpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#ifndef SPRITE_RENDERER_HPP
#define SPRITE_RENDERER_HPP

#include <glm/vec4.hpp>

#include "Engine2D/Component2D.hpp"

namespace Engine::Rendering {
  class Shader;
}

namespace Engine2D::Rendering {
  class Sprite;
  class SpriteRenderer final : public Component2D {
    public:
      std::shared_ptr<Engine::Rendering::Shader> shader;
      std::shared_ptr<Sprite> sprite;
      glm::vec4 color;
      uint32_t renderLayer;
      int renderOrder;

      SpriteRenderer();
  };
}

#endif //SPRITE_RENDERER_HPP
