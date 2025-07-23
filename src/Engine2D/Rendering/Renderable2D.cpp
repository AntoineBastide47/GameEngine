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
    : shader(Engine::ResourceManager::GetShader("sprite")), sprite(nullptr), renderOrder(0) {}

  void Renderable2D::SetShader(Shader *shader) {
    this->shader = shader;
  }

  Shader *Renderable2D::GetShader() const {
    return shader;
  }

  void Renderable2D::SetSprite(Sprite *sprite) {
    this->sprite = sprite;
  }

  Sprite *Renderable2D::GetSprite() const {
    return sprite;
  }

  void Renderable2D::SetRenderOrder(const int16_t renderOrder) {
    this->renderOrder = renderOrder;
  }

  int16_t Renderable2D::GetRenderOrder() const {
    return renderOrder;
  }
}
