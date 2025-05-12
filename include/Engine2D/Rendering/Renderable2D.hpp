//
// Renderable2D.hpp
// Author: Antoine Bastide
// Date: 05/05/2025
//

#ifndef RENDERABLE2D_HPP
#define RENDERABLE2D_HPP

#include <memory>

#include "Engine2D/Component2D.hpp"

namespace Engine::Rendering {
  class Shader;
}

namespace Engine2D::Rendering {
  class Sprite;
  class Renderable2D : public Component2D {
    friend class Renderer2D;
    public:
      Renderable2D();

      /// Sets the shader that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetShader(const std::shared_ptr<Engine::Rendering::Shader>& shader);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] std::shared_ptr<Engine::Rendering::Shader> GetShader() const;
      /// Sets the sprite that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetSprite(const std::shared_ptr<Sprite> &sprite);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] std::shared_ptr<Sprite> GetSprite() const;
      /// Sets the render order that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetRenderOrder(int16_t renderOrder);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] int16_t GetRenderOrder() const;
    protected:
      std::shared_ptr<Engine::Rendering::Shader> shader;
      std::shared_ptr<Sprite> sprite;
      int16_t renderOrder;
  };
}

#endif //RENDERABLE2D_HPP
