//
// SpriteRenderer.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "2D/Rendering/SpriteRenderer.h"

namespace Engine2D::Rendering {
  SpriteRenderer::SpriteRenderer(Shader *shader) : quadVAO(0) {
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

  void SpriteRenderer::DrawSprite(const Entity2D *entity, const glm::vec3 color) const {
    // prepare transformations
    this->shader->Use();
    auto model = glm::mat4(1.0f);
    model = translate(model, glm::vec3(entity->transform.position.toGLM(), 0.0f));
    // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = translate(model, glm::vec3(entity->transform.scale.toGLM() * 0.5f, 0.0f));    // move origin of rotation to center of quad
    model = rotate(model, glm::radians(entity->transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = translate(model, glm::vec3(entity->transform.scale.toGLM() * -0.5f, 0.0f));  // move origin back

    model = scale(model, glm::vec3(entity->transform.scale.toGLM(), 1.0f)); // last scale

    this->shader->SetMatrix4("model", model);

    // render textured quad
    this->shader->SetVector3f("spriteColor", color);

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
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), /*(void *) 0*/ nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}
