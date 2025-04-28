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

namespace Engine2D {
  class Entity2D;
}

namespace Engine2D::Rendering {
  class Sprite;
  class SpriteRenderer final : public Component2D {
    friend class Renderer2D;
    friend class Engine2D::Entity2D;
    public:
      SpriteRenderer();

      /// Sets the shader used to render this sprite
      void SetShader(const std::shared_ptr<Engine::Rendering::Shader> &shader);
      /// @returns The shader used to render this sprite
      std::shared_ptr<Engine::Rendering::Shader> GetShader() const;
      /// Sets the sprite to render
      void SetSprite(const std::shared_ptr<Sprite> &sprite);
      /// @returns the sprite that will be rendered
      std::shared_ptr<Sprite> GetSprite() const;
      /// Sets the color of the sprite
      void SetColor(const glm::vec4 &color);
      /// @returns the color of the sprite
      const glm::vec4 &GetColor() const;
      /// Sets on which axis to flip the sprite
      void SetFlip(const glm::vec<2, bool> &flip);
      /// @returns on which axis the sprite is flipped
      const glm::vec<2, bool> &GetFlip() const;
      /// Sets the render order of this sprite
      void SetRenderOrder(int16_t order);
      /// @returns the render order of this sprite
      size_t GetRenderOrder() const;
    private:
      /// The shader used to render the target sprite
      std::shared_ptr<Engine::Rendering::Shader> shader;
      /// The sprite to render
      std::shared_ptr<Sprite> sprite;
      /// The color of the sprite
      glm::vec4 color;
      /// If the sprite should be flipped on a given axis
      glm::vec<2, bool> flip;
      /// Rendering priority: higher value means higher priority
      int16_t renderOrder;
      /// Whether the sprite renderer was updated
      bool dirty;
      /// Whether the sprite of the renderer was updated
      bool spriteDirty;
  };
}

#endif //SPRITE_RENDERER_HPP
