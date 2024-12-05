//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "2D/Rendering/SpriteRenderer.h"
#include "2D/Game2D.h"

namespace Engine2D::Rendering {
  Shader *SpriteRenderer::shader = nullptr;
  unsigned int SpriteRenderer::quadVAO{};
  unsigned int SpriteRenderer::lastBoundTexture{};

  SpriteRenderer::~SpriteRenderer() {
    if (quadVAO > 0) {
      glDeleteVertexArrays(1, &quadVAO);
      quadVAO = 0;
    }
  }

  void SpriteRenderer::DrawSprite(const Entity2D *entity) {
    // Simple check that makes sure the entity is visible
    if (!entity->transform.IsInScreenBounds())
      return;

    // render textured quad
    shader->SetMatrix4("model", entity->transform.TransformMatrix());
    shader->SetVector3f("spriteColor", entity->textureColor);

    glActiveTexture(GL_TEXTURE0);
    if (entity->texture->id != lastBoundTexture) {
      glActiveTexture(GL_TEXTURE0);
      entity->texture->Bind();
      lastBoundTexture = entity->texture->id;
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }

  void SpriteRenderer::initRenderData() {
    constexpr float vertices[] = {
      // pos      // tex
      -0.5f, 0.5f, 0.0f, 1.0f,  // Top-left corner
      0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right corner
      -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left corner

      -0.5f, 0.5f, 0.0f, 1.0f, // Top-left corner
      0.5f, 0.5f, 1.0f, 1.0f,  // Top-right corner
      0.5f, -0.5f, 1.0f, 0.0f  // Bottom-right corner
    };

    glGenVertexArrays(1, &quadVAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}
