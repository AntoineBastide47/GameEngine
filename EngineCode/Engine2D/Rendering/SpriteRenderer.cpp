//
// SpriteRenderer.cpp
// Author: Antoine Bastide (modified for batching)
// Date: 10/11/2024 (modified 02/03/2025)
//

#include <iostream>
#include <vector>
#include <map>

#include "Engine2D/Rendering/SpriteRenderer.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Rendering/Shader.h"

namespace Engine2D::Rendering {
  const float SpriteRenderer::vertices[24] = {
    // pos         // tex
    -0.5f,  0.5f,  0.0f, 1.0f,  // Top-left corner
     0.5f, -0.5f,  1.0f, 0.0f,  // Bottom-right corner
    -0.5f, -0.5f,  0.0f, 0.0f,  // Bottom-left corner

    -0.5f,  0.5f,  0.0f, 1.0f,  // Top-left corner
     0.5f,  0.5f,  1.0f, 1.0f,  // Top-right corner
     0.5f, -0.5f,  1.0f, 0.0f   // Bottom-right corner
  };

  // Static members
  std::shared_ptr<Shader> SpriteRenderer::shader = nullptr;
  uint SpriteRenderer::quadVAO{};
  uint SpriteRenderer::batchVBO{};
  uint SpriteRenderer::lastBoundTexture{};

  SpriteRenderer::~SpriteRenderer() {
    if (quadVAO > 0) {
      glDeleteVertexArrays(1, &quadVAO);
      quadVAO = 0;
    }
    if (batchVBO > 0) {
      glDeleteBuffers(1, &batchVBO);
      batchVBO = 0;
    }
  }

  void SpriteRenderer::initRenderData() {
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

    // The stride (in bytes) for one instance is 20 floats:
    constexpr GLsizei stride = 20 * sizeof(float);
    constexpr GLsizei vec4Size = 4 * sizeof(float);

    // Attribute locations 1–4 will hold the 4 columns of the model matrix.
    // (Remember: OpenGL expects matrices to be supplied as 4 vec4 attributes.)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, nullptr);
    glVertexAttribDivisor(1, 1); // Advance once per instance

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)vec4Size);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * vec4Size));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * vec4Size));
    glVertexAttribDivisor(4, 1);

    // Attribute location 5: sprite color (vec4)
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * vec4Size));
    glVertexAttribDivisor(5, 1);

    // Cleanup: unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void SpriteRenderer::render(std::map<int, std::vector<std::shared_ptr<Entity2D>>> &entities) {
    if (!shader)
      return;

    if (!quadVAO)
      initRenderData();

    shader->Use();
    glBindVertexArray(quadVAO);

    for (const auto [id, group] : entities) {
      if (group.empty()) {
        entities.erase(id);
        continue;
      }

      // Bind the texture for this batch.
      group.front()->Texture()->bind();

      // Build a temporary array to hold the per-instance data.
      // Each valid sprite contributes 20 floats.
      std::vector<float> batchData;
      batchData.reserve(group.size() * 20);

      int instanceCount = 0;
      for (const auto &entity : group) {
        if (!entity || !entity->IsActive() || !entity->transform->GetIsVisible())
          continue;

        // Get the model matrix
        const auto &modelMatrix = entity->transform->GetWorldMatrix();
        // Append the 16 floats (column-major order) from the model matrix.
        const float* matrixPtr = glm::value_ptr(modelMatrix);
        batchData.insert(batchData.end(), matrixPtr, matrixPtr + 16);

        // Append the sprite color
        batchData.push_back(entity->textureColor.r);
        batchData.push_back(entity->textureColor.g);
        batchData.push_back(entity->textureColor.b);
        batchData.push_back(entity->textureColor.a);

        instanceCount++;
      }

      // If no sprites in this group are active/visible, skip drawing.
      if (instanceCount == 0)
        continue;

      // Update the instance VBO with the new data.
      glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
      glBufferData(GL_ARRAY_BUFFER, batchData.size() * sizeof(float), batchData.data(), GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // Draw all sprites in the batch in a single call.
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, instanceCount);
    }
    glBindVertexArray(0);
  }
}
