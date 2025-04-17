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
    friend class Renderer2D;
    public:
      SpriteRenderer();

      void SetShader(const std::shared_ptr<Engine::Rendering::Shader> &shader);
      std::shared_ptr<Engine::Rendering::Shader> GetShader() const;
      void SetSprite(const std::shared_ptr<Sprite> &sprite);
      std::shared_ptr<Sprite> GetSprite() const;
      void SetColor(const glm::vec4 &color);
      const glm::vec4 &GetColor() const;
      void SetFlip(glm::vec<2, bool> flip);
      const glm::vec<2, bool> &GetFlip() const;
      void SetRenderOrder(size_t order);
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
  };
}

#endif //SPRITE_RENDERER_HPP
