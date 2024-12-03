//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "2D/Rendering/SpriteRenderer.h"

#include <iostream>

#include "2D/Game2D.h"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer(Shader *shader)
    : quadVAO(0) {
    if (!shader)
      throw std::invalid_argument("ERROR::SPRITE_RENDERER: NULL shader passed");
    this->shader = shader;
    this->initRenderData();
  }

  SpriteRenderer::~SpriteRenderer() {
    if (this->quadVAO > 0) {
      glDeleteVertexArrays(1, &this->quadVAO);
      this->quadVAO = 0;
    }
  }

  void SpriteRenderer::DrawSprite(const Entity2D *entity) const {
    // Simple check that makes sure the entity is visible
    const bool visible = !(
      entity->transform.position.x + entity->transform.scale.x * 0.5f < -static_cast<float>(Game2D::Width()) * 0.5f ||
      entity->transform.position.y - entity->transform.scale.y * 0.5f > static_cast<float>(Game2D::Height()) * 0.5f ||
      entity->transform.position.x - entity->transform.scale.x * 0.5f > static_cast<float>(Game2D::Width()) * 0.5f ||
      entity->transform.position.y + entity->transform.scale.y * 0.5f < -static_cast<float>(Game2D::Height()) * 0.5f
    );

    if (!visible)
      return;

    this->shader->Use();
    auto model = glm::mat4(1.0f);

    // Transform then scale and finally rotate the identity matrix
    model = translate(model, glm::vec3(entity->transform.WorldPosition().toGLM(), 0.0f));
    model = rotate(model, glm::degrees(-entity->transform.WorldRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
    model = scale(model, glm::vec3(-entity->transform.WorldScale().toGLM(), 1.0f));

    // render textured quad
    this->shader->SetMatrix4("model", model);
    this->shader->SetVector3f("spriteColor", entity->textureColor);

    glActiveTexture(GL_TEXTURE0);
    entity->texture->Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }

  void SpriteRenderer::initRenderData() {
    // configure VAO/VBO
    unsigned int VBO;
    constexpr float vertices[] = {
      // pos      // tex
      -0.5f, 0.5f, 0.0f, 1.0f,  // Top-left corner
      0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right corner
      -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left corner

      -0.5f, 0.5f, 0.0f, 1.0f, // Top-left corner
      0.5f, 0.5f, 1.0f, 1.0f,  // Top-right corner
      0.5f, -0.5f, 1.0f, 0.0f  // Bottom-right corner
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}
