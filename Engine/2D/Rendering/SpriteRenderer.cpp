//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "2D/Rendering/SpriteRenderer.h"
#include "2D/Game2D.h"

namespace Engine2D::Rendering {
  std::shared_ptr<Shader> SpriteRenderer::shader = nullptr;
  unsigned int SpriteRenderer::quadVAO{};
  unsigned int SpriteRenderer::lastBoundTexture{};

  SpriteRenderer::~SpriteRenderer() {
    if (quadVAO > 0) {
      glDeleteVertexArrays(1, &quadVAO);
      quadVAO = 0;
    }
  }

  void SpriteRenderer::drawSprites(const std::map<int, std::unordered_set<std::shared_ptr<Entity2D> > > &entities) {
    if (!shader)
      return;

    shader->Use();
    for (const auto [id, e]: entities) {
      // Render texture
      glActiveTexture(GL_TEXTURE0);
      if (auto it = entities.at(id).begin(); it != entities.at(id).end())
        (*it)->Texture()->bind();

      for (const auto entity: e) {
        if (!entity || !entity->active || !entity->Texture() || !entity->transform.Visible())
          continue;

        // render textured quad
        shader->SetMatrix4("model", entity->transform.WorldMatrix());
        shader->SetVector3f("spriteColor", entity->textureColor);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
      }
    }
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
