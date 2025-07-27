//
// Sprite.hpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <glm/glm.hpp>

#include "Sprite.gen.hpp"

namespace Engine::Rendering {
  class Texture;
}

using Engine::Rendering::Texture;

namespace Engine2D::Rendering {
  class Sprite : public Engine::Reflection::Reflectable {
    SERIALIZE_SPRITE
    public:
      Sprite() = default;

      /// The texture used for rendering this sprite.
      Texture *texture;

      /**
       * The subregion of the texture to use.
       * Format: (u, v, width, height), in normalized UV space [0.0, 1.0].
       * Use (0, 0, 1, 1) to use the full texture.
       */
      glm::vec<4, float> rect{0, 0, 1, 1};

      /**
       * Pivot point of the sprite, in normalized local space.
       * (0, 0) = bottom-left, (0.5, 0.5) = center, (1, 1) = top-right.
       */
      glm::vec<2, float> pivot{0.5f, 0.5f};

      /**
       * Pixels per world unit.
       * Controls the visual size of the sprite in world space.
       * For example, 100 means 100 pixels in the texture = 1 unit in the world.
       */
      float pixelsPerUnit{1.0f};

      /// Set to true if the sprite's texture contains transparent pixels, false if not
      bool transparent{false};
  };
}

#endif //SPRITE_HPP
