//
// Renderable2D.hpp
// Author: Antoine Bastide
// Date: 05/05/2025
//

#ifndef RENDERABLE2D_HPP
#define RENDERABLE2D_HPP

#include <memory>

#include "Engine2D/Component2D.hpp"
#include "Renderable2D.gen.hpp"

namespace Engine::Rendering {
  class Shader;
}

namespace Engine2D::Rendering {
  class Sprite;

  class Renderable2D : public Component2D {
    SERIALIZE_RENDERABLE2D
      friend class Renderer2D;
    public:
      enum RenderableType {
        None, SpriteRenderer, ParticleSystem
      };
        REFLECT_RENDERABLETYPE

      /// Sets the shader that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetShader(Engine::Rendering::Shader *shader);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] Engine::Rendering::Shader *Shader() const;
      /// Sets the sprite that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetSprite(Sprite *sprite);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] Sprite *Sprite() const;
      /// Sets the render order that will be used for rendering
      /// @note Will not work outside OnInitialize if the entity is marked as static
      void SetRenderOrder(int16_t renderOrder);
      /// @returns the shader that will be used for rendering
      [[nodiscard]] int16_t RenderOrder() const;
    protected:
      /// The order in which to render this renderable
      ENGINE_SERIALIZE int16_t renderOrder;
      /// The type of renderable to render
      ENGINE_SERIALIZE_HIDDEN RenderableType renderType;
      /// The shader tied to this renderable
      ENGINE_SHOW_IN_INSPECTOR Engine::Rendering::Shader *shader;
      /// The sprite tied to this renderable
      ENGINE_SHOW_IN_INSPECTOR Rendering::Sprite *sprite;
      /// Whether the sprite renderer was updated
      bool dirty;

      Renderable2D();
      explicit Renderable2D(RenderableType type);

      void OnSerialize(Engine::Reflection::Format format, Engine::JSON &json) const override;
      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
      void OnEditorValueChanged() override;
  };
}

#endif //RENDERABLE2D_HPP
