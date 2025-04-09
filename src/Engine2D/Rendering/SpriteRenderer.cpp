//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 09/04/2025
//

#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine/ResourceManager.hpp"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer() : color(1), renderLayer(0), renderOrder(0) {
    shader = Engine::ResourceManager::GetShader("sprite");
  }
}
