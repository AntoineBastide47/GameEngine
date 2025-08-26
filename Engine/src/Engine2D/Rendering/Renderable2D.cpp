//
// Renderable2D.cpp
// Author: Antoine Bastide
// Date: 05/05/2025
//

#include "Engine2D/Rendering/Renderable2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine2D/Entity2D.hpp"

namespace Engine2D::Rendering {
  Renderable2D::Renderable2D()
    : Renderable2D(None) {}

  Renderable2D::Renderable2D(const RenderableType type)
    : renderOrder(0), renderType(type), shader(Engine::ResourceManager::GetShader("sprite")), sprite(nullptr),
      dirty(true) {}

  void Renderable2D::SetShader(Engine::Rendering::Shader *shader) {
    this->shader = shader;
    dirty = true;
  }

  Shader *Renderable2D::Shader() const {
    return shader;
  }

  void Renderable2D::SetSprite(Rendering::Sprite *sprite) {
    this->sprite = sprite;
    dirty = true;
  }

  Sprite *Renderable2D::Sprite() const {
    return sprite;
  }

  void Renderable2D::SetRenderOrder(const int16_t renderOrder) {
    this->renderOrder = renderOrder;
    dirty = true;
  }

  int16_t Renderable2D::RenderOrder() const {
    return renderOrder;
  }

  void Renderable2D::OnSerialize(const Engine::Reflection::Format format, Engine::JSON &json) const {
    if (format == Engine::Reflection::Format::JSON) {
      json["shader"] = Engine::ResourceManager::GetShaderName(shader);
      json["sprite"] = Engine::ResourceManager::GetSpriteName(sprite);
    }
  }

  void Renderable2D::OnDeserialize(const Engine::Reflection::Format format, const Engine::JSON &json) {
    if (format == Engine::Reflection::Format::JSON) {
      shader = Engine::ResourceManager::GetShader(json.At("shader").GetString());
      sprite = Engine::ResourceManager::GetSprite(json.At("sprite").GetString());
    }
  }

  void Renderable2D::OnEditorValueChanged() {
    dirty = true;
  }
}
