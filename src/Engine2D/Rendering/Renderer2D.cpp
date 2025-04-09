//
// SpriteRenderer.cpp
// Author: Antoine Bastide (modified for batching)
// Date: 10/11/2024 (modified 02/03/2025)
//

#include <iostream>
#include <vector>
#include <map>

#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine/Rendering/Shader.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"

namespace Engine2D::Rendering {
  const float Renderer2D::vertices[24] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
    0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left

    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 1.0f, 1.0f, 1.0f, // Top-right
    1.0f, 0.0f, 1.0f, 0.0f  // Bottom-right
  };

  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderers;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderersToAdd;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderersToRemove;

  uint Renderer2D::quadVAO{};
  uint Renderer2D::batchVBO{};
  uint Renderer2D::lastBoundTexture{};

  uint Renderer2D::instanceCount;
  uint Renderer2D::lastShaderID;
  uint Renderer2D::lastTextureID;
  std::vector<float> Renderer2D::batchData;

  Renderer2D::Renderer2D() {
    batchData.reserve(MAX_BATCH_SIZE);
  }

  Renderer2D::~Renderer2D() {
    if (quadVAO > 0) {
      glDeleteVertexArrays(1, &quadVAO);
      quadVAO = 0;
    }
    if (batchVBO > 0) {
      glDeleteBuffers(1, &batchVBO);
      batchVBO = 0;
    }
    renderers.clear();
    renderersToAdd.clear();
    renderersToRemove.clear();
  }

  void Renderer2D::initRenderData() {
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // --- Set up the quad’s vertex buffer ---
    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0: vertex data (vec4: position.xy, texCoords.xy)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // --- Set up the instance buffer ---
    // We are going to store, for each sprite, 20 floats:
    //  • 16 floats for the model matrix (4 columns of vec4)
    //  •  4 floats for the sprite color.
    glGenBuffers(1, &batchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // The stride (in bytes) for one instance
    constexpr GLsizei stride = 27 * sizeof(float);
    constexpr GLsizei vec4Size = 4 * sizeof(float);

    // Attribute locations 1–4 will hold the 4 columns of the model matrix.
    // (Remember: OpenGL expects matrices to be supplied as 4 vec4 attributes.)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, nullptr);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(vec4Size));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(2 * vec4Size));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(3 * vec4Size));
    glVertexAttribDivisor(4, 1);

    // Attribute location 5: sprite color (vec4)
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(4 * vec4Size));
    glVertexAttribDivisor(5, 1);

    // Attribute location 6: sprite rect
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(5 * vec4Size));
    glVertexAttribDivisor(6, 1);

    // Attribute location 6: sprite pivot
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(6 * vec4Size + 0 * sizeof(float)));
    glVertexAttribDivisor(7, 1);

    // Attribute location 6: sprite pixelsPerUnit
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(6 * vec4Size + 2 * sizeof(float)));
    glVertexAttribDivisor(8, 1);

    // Cleanup: unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void Renderer2D::render() {
    // Add all pending renderers
    for (auto &renderer: renderersToAdd)
      renderers.emplace_back(renderer);
    // Remove all pending renderers
    for (auto &renderer: renderersToRemove)
      std::erase(renderers, renderer);

    if (!renderersToAdd.empty()) {
      std::ranges::sort(
        renderers, [](
        const std::shared_ptr<SpriteRenderer> &a, const std::shared_ptr<SpriteRenderer> &b
      ) {
          const bool aInvalid = !a->shader || !a->sprite || !a->sprite->texture;

          if (const bool bInvalid = !b->shader || !b->sprite || !b->sprite->texture; aInvalid != bInvalid)
            return !aInvalid;

          return std::tie(a->renderLayer, a->renderOrder, a->shader->id, a->sprite->texture->id) <
                 std::tie(b->renderLayer, b->renderOrder, b->shader->id, b->sprite->texture->id);
        }
      );
    }
    renderersToAdd.clear();
    renderersToRemove.clear();

    if (renderers.empty())
      return;

    if (!quadVAO)
      initRenderData();

    glBindVertexArray(quadVAO);
    lastShaderID = 0;
    lastTextureID = 0;

    for (const auto spriteRenderer: renderers) {
      if (!spriteRenderer->shader || !spriteRenderer->sprite->texture)
        continue;

      // Bind a new shader if needed
      if (lastShaderID != spriteRenderer->shader->id && spriteRenderer->shader->id > 0) {
        flush();
        lastShaderID = spriteRenderer->shader->id;
        spriteRenderer->shader->Use();
      }

      // Bind a new texture if needed
      if (lastTextureID != spriteRenderer->sprite->texture->id && spriteRenderer->sprite->texture->id > 0) {
        flush();
        lastTextureID = spriteRenderer->sprite->texture->id;
        spriteRenderer->sprite->texture->bind();
      }

      const auto entity = spriteRenderer->Entity();

      if (!entity || !entity->IsActive() || !entity->transform->GetIsVisible())
        continue;

      // Get the model matrix
      const auto &modelMatrix = entity->transform->GetWorldMatrix();
      // Append the 16 floats (column-major order) from the model matrix.
      const float *matrixPtr = glm::value_ptr(modelMatrix);
      batchData.insert(batchData.end(), matrixPtr, matrixPtr + 16);

      // Append the sprite color
      batchData.push_back(spriteRenderer->color.r);
      batchData.push_back(spriteRenderer->color.g);
      batchData.push_back(spriteRenderer->color.b);
      batchData.push_back(spriteRenderer->color.a);
      batchData.push_back(spriteRenderer->sprite->rect.x);
      batchData.push_back(spriteRenderer->sprite->rect.y);
      batchData.push_back(spriteRenderer->sprite->rect.z);
      batchData.push_back(spriteRenderer->sprite->rect.w);

      // Add pivot
      batchData.push_back(spriteRenderer->sprite->pivot.x);
      batchData.push_back(spriteRenderer->sprite->pivot.y);

      // Add pixelsPerUnit
      batchData.push_back(spriteRenderer->sprite->pixelsPerUnit);

      instanceCount++;

      if (instanceCount >= MAX_INSTANCE_COUNT)
        flush();
    }

    // Draw any leftover sprites
    if (instanceCount > 0) {
      flush();
      glBindVertexArray(0);
    }
  }

  void Renderer2D::flush() {
    // Update the instance VBO with the new data.
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBufferData(GL_ARRAY_BUFFER, batchData.size() * sizeof(float), batchData.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Draw all sprites in the batch in a single call.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, instanceCount);

    batchData.clear();
    instanceCount = 0;
  }

  void Renderer2D::addRenderer(const std::shared_ptr<SpriteRenderer> &renderer) {
    renderersToAdd.emplace_back(renderer);
  }

  void Renderer2D::removeRenderer(const std::shared_ptr<SpriteRenderer> &renderer) {
    renderersToRemove.emplace_back(renderer);
  }
}
