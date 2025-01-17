//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "Engine2D/Rendering/SpriteRenderer.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Rendering/Shader.h"

namespace Engine2D::Rendering {
  const float SpriteRenderer::vertices[24] = {
    // pos      // tex
    -0.5f, 0.5f, 0.0f, 1.0f,  // Top-left corner
    0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right corner
    -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left corner

    -0.5f, 0.5f, 0.0f, 1.0f, // Top-left corner
    0.5f, 0.5f, 1.0f, 1.0f,  // Top-right corner
    0.5f, -0.5f, 1.0f, 0.0f  // Bottom-right corner
  };

  std::shared_ptr<Shader> SpriteRenderer::shader = nullptr;
  unsigned int SpriteRenderer::quadVAO{};
  unsigned int SpriteRenderer::lastBoundTexture{};

  SpriteRenderer::~SpriteRenderer() {
    if (quadVAO > 0) {
      glDeleteVertexArrays(1, &quadVAO);
      quadVAO = 0;
    }
  }

  void SpriteRenderer::initRenderData() {
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // Single-quad VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set the data to pass to the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void SpriteRenderer::render(const std::map<int, std::unordered_set<std::shared_ptr<Entity2D>>> &entities) {
    if (!shader)
      return;

    shader->Use();
    glBindVertexArray(quadVAO);

    for (const auto [id, e]: entities) {
      // Render texture
      e.begin()->get()->Texture()->bind();

      for (const auto entity: e) {
        if (!entity || !entity->IsActive() || !entity->transform.Visible())
          continue;

        // render textured quad
        shader->SetMatrix4("model", entity->transform.WorldMatrix());
        shader->SetVector4f("spriteColor", entity->textureColor);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
      }
    }
    glBindVertexArray(0);
  }
}
