//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer()
    : Renderable2D(RenderableType::SpriteRenderer), color({1, 1, 1, 1}), flip({0, 0}) {}

  void SpriteRenderer::SetColor(const glm::vec<4, float> &color) {
    this->color = color;
    dirty = true;
  }

  const glm::vec<4, float> &SpriteRenderer::Color() const {
    return this->color;
  }

  void SpriteRenderer::SetFlip(const glm::vec<2, bool> &flip) {
    this->flip = flip;
    dirty = true;
  }

  const glm::vec<2, bool> &SpriteRenderer::Flip() const {
    return this->flip;
  }

  void SpriteRenderer::forward() {
    Entity()->Scene()->renderingSystem.addRenderer(this);
  }

  void SpriteRenderer::recall() {
    Entity()->Scene()->renderingSystem.removeRenderer(this);
  }
}
