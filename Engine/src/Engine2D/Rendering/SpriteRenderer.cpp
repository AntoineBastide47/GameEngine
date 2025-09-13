//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer()
    : Renderable2D(RenderableType::SpriteRenderer), color(Engine::Rendering::Color::White()), flip({0, 0}) {}

  void SpriteRenderer::SetColor(const Engine::Rendering::Color &color) {
    this->color = color;
    dirty = true;
  }

  const Engine::Rendering::Color &SpriteRenderer::Color() const {
    return color;
  }

  void SpriteRenderer::SetFlip(const glm::vec<2, bool> &flip) {
    this->flip = flip;
    dirty = true;
  }

  const glm::vec<2, bool> &SpriteRenderer::Flip() const {
    return flip;
  }

  void SpriteRenderer::forward() {
    Entity()->Scene()->renderingSystem.addRenderer(this);
  }

  void SpriteRenderer::recall() {
    Entity()->Scene()->renderingSystem.removeRenderer(this);
  }
}
