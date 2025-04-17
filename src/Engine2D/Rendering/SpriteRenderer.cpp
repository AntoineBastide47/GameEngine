//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine/ResourceManager.hpp"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer()
    : color(1), flip({0, 0}), renderOrder(0), dirty(false) {
    shader = Engine::ResourceManager::GetShader("sprite");
  }

  void SpriteRenderer::SetShader(const std::shared_ptr<Shader> &shader) {
    this->shader = shader;
    this->dirty = true;
  }

  std::shared_ptr<Shader> SpriteRenderer::GetShader() const {
    return this->shader;
  }

  void SpriteRenderer::SetSprite(const std::shared_ptr<Sprite> &sprite) {
    this->sprite = sprite;
    this->dirty = true;
  }

  std::shared_ptr<Sprite> SpriteRenderer::GetSprite() const {
    return this->sprite;
  }

  void SpriteRenderer::SetColor(const glm::vec4 &color) {
    this->color = color;
    this->dirty = true;
  }

  const glm::vec4 &SpriteRenderer::GetColor() const {
    return this->color;
  }

  void SpriteRenderer::SetFlip(const glm::vec<2, bool> flip) {
    this->flip = flip;
    this->dirty = true;
  }

  const glm::vec<2, bool> &SpriteRenderer::GetFlip() const {
    return this->flip;
  }

  void SpriteRenderer::SetRenderOrder(size_t order) {
    this->renderOrder = order;
    this->dirty = true;
  }

  size_t SpriteRenderer::GetRenderOrder() const {
    return this->renderOrder;
  }
}
