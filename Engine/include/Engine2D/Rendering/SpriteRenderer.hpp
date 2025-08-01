//
// SpriteRenderer.hpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#ifndef SPRITE_RENDERER_HPP
#define SPRITE_RENDERER_HPP

#include <glm/glm.hpp>

#include "Engine/Types/float01.hpp"
#include "Engine2D/Rendering/Renderable2D.hpp"
#include "SpriteRenderer.gen.hpp"

namespace Engine::Rendering {
  class Shader;
}

namespace Engine2D {
  class Entity2D;
}

namespace Engine2D::Rendering {
  class Sprite;

  class SpriteRenderer final : public Renderable2D {
    SERIALIZE_SPRITERENDERER
      friend class Renderer2D;
      friend class Engine2D::Entity2D;
    public:
      /// Sets the color of the sprite
      void SetColor(const glm::vec4 &color);
      /// @returns the color of the sprite
      const glm::vec4 &GetColor() const;
      /// Sets on which axis to flip the sprite
      void SetFlip(const glm::vec<2, bool> &flip);
      /// @returns on which axis the sprite is flipped
      const glm::vec<2, bool> &GetFlip() const;
    private:
      /// The color of the sprite
      ENGINE_SERIALIZE glm::vec4 color;
      /// If the sprite should be flipped on a given axis
      ENGINE_SERIALIZE glm::vec<2, bool> flip;
      /// Whether the sprite renderer was updated
      bool dirty;

      SpriteRenderer();

      void forward() override;
      void recall() override;
  };
}

#endif //SPRITE_RENDERER_HPP
