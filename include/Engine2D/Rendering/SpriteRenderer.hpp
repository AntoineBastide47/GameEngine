//
// SpriteRenderer.hpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#ifndef SPRITE_RENDERER_HPP
#define SPRITE_RENDERER_HPP

#include <glm/glm.hpp>

#include "Engine2D/Component2D.hpp"

namespace Engine::Rendering {
  class Shader;
}

namespace Engine2D::Rendering {
  class Sprite;
  class SpriteRenderer final : public Component2D {
    public:
      /// The shader used to render the target sprite
      std::shared_ptr<Engine::Rendering::Shader> shader;
      /// The sprite to render
      std::shared_ptr<Sprite> sprite;
      /// The color of the sprite
      glm::vec4 color;
      /// If the sprite should be flipped on a given axis
      glm::vec<2, bool> flip;
      /// The layer on which to render the sprite
      uint32_t renderLayer;
      /// The ordering of the sprite within the render layer
      int renderOrder;

      SpriteRenderer();
  };
}

#endif //SPRITE_RENDERER_HPP
