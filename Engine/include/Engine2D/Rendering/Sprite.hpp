//
// Sprite.hpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <glm/glm.hpp>

namespace Engine::Rendering {
  class Texture;
}

namespace Engine {
  class ResourceManager;
}

using Engine::Rendering::Texture;

namespace Engine2D::Rendering {
  class Sprite final {
    friend class Engine::ResourceManager;
    public:
      Sprite() = default;

      /// The texture used for rendering this sprite.
      Texture *texture;

      /**
       * The subregion of the texture to use.
       * Format: (u, v, width, height), in normalized UV space [0.0, 1.0].
       * Use (0, 0, 1, 1) to use the full texture.
       */
      glm::vec4 rect{0, 0, 1, 1};

      /**
       * Pivot point of the sprite, in normalized local space.
       * (0, 0) = bottom-left, (0.5, 0.5) = center, (1, 1) = top-right.
       */
      glm::vec2 pivot{0.5f, 0.5f};

      /**
       * Pixels per world unit.
       * Controls the visual size of the sprite in world space.
       * For example, 100 means 100 pixels in the texture = 1 unit in the world.
       */
      float pixelsPerUnit{1.0f};

      /// Set to true if the sprite's texture contains transparent pixels, false if not
      bool transparent{false};

      /// @returns the name of this sprite
      const std::string &Name() const {
        return name;
      }
    private:
      /// The name of this sprite
      std::string name;
  };
}

#endif //SPRITE_HPP
