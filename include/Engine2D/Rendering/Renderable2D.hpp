//
// Renderable2D.hpp
// Author: Antoine Bastide
// Date: 05/05/2025
//

#ifndef RENDERABLE2D_HPP
#define RENDERABLE2D_HPP

#include <memory>

#include "Engine/Rendering/Shader.hpp"
#include "Engine2D/Component2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Renderable2D.gen.hpp"

namespace Engine2D::Rendering {
  class Renderable2D : public Component2D {
    SERIALIZE_RENDERABLE2D
      friend class Renderer2D;
    public:
      Renderable2D();

      /// Sets the shader that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetShader(Engine::Rendering::Shader *shader);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] Engine::Rendering::Shader *GetShader() const;
      /// Sets the sprite that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetSprite(Sprite *sprite);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] Sprite *GetSprite() const;
      /// Sets the render order that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetRenderOrder(int16_t renderOrder);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] int16_t GetRenderOrder() const;
    protected:
      Engine::Rendering::Shader *shader;
      Sprite *sprite;
      int16_t renderOrder;
  };
}

#endif //RENDERABLE2D_HPP
