//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer()
    : color({1, 1, 1, 1}), flip({0, 0}), dirty(false) {}

  void SpriteRenderer::SetColor(const glm::vec<4, float> &color) {
    this->color = color;
    dirty = true;
  }

  const glm::vec<4, float> &SpriteRenderer::GetColor() const {
    return this->color;
  }

  void SpriteRenderer::SetFlip(const glm::vec<2, bool> &flip) {
    this->flip = flip;
    dirty = true;
  }

  const glm::vec<2, bool> &SpriteRenderer::GetFlip() const {
    return this->flip;
  }

  void SpriteRenderer::forward() {
    Renderer2D::addRenderer(this);
  }

  void SpriteRenderer::recall() {
    Renderer2D::removeRenderer(this);
  }
}
