//
// SpriteRenderer.cpp
// Author: Antoine Bastide (modified for batching)
// Date: 10/11/2024 (modified 02/03/2025)
//

#include <vector>

#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine/Rendering/Shader.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine/Rendering/Texture.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"

namespace Engine2D::Rendering {
  const float Renderer2D::vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
    0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left

    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 1.0f, 1.0f, 1.0f, // Top-right
    1.0f, 0.0f, 1.0f, 0.0f  // Bottom-right
  };

  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderers;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::staticRenderers;

  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderersToAdd;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderersToRemove;

  uint Renderer2D::quadVAO{};
  uint Renderer2D::batchVBO{};
  uint Renderer2D::staticBatchVBO{};
  uint Renderer2D::lastBoundTexture{};

  uint Renderer2D::instanceCount;
  uint Renderer2D::lastShaderID;
  uint Renderer2D::lastTextureID;

  std::vector<float> Renderer2D::batchData;
  std::vector<float> Renderer2D::staticBatchData;
  std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> Renderer2D::staticFlushList;

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
    if (staticBatchVBO > 0) {
      glDeleteBuffers(1, &staticBatchVBO);
      staticBatchVBO = 0;
    }

    renderersToAdd.clear();
    renderersToRemove.clear();

    renderers.clear();
    batchData.clear();

    staticRenderers.clear();
    staticBatchData.clear();
    staticFlushList.clear();
  }

  void Renderer2D::initRenderData() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // --- Set up the quad’s vertex buffer ---
    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0: position and texCoord
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // --- Set up the instance buffer ---
    glGenBuffers(1, &batchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Setup instance attributes:
    constexpr GLsizei vec4Size = 4 * sizeof(float);
    // Attribute location 1: scale and rotation
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, nullptr);
    glVertexAttribDivisor(1, 1);
    // Attribute location 2: position and sprite pivot
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(vec4Size));
    glVertexAttribDivisor(2, 1);
    // Attribute location 3: sprite color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(2 * vec4Size));
    glVertexAttribDivisor(3, 1);
    // Attribute location 4: sprite rect
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(3 * vec4Size));
    glVertexAttribDivisor(4, 1);
    // Attribute location 5: sprite renderOrder and pixelPerUnit
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(4 * vec4Size));
    glVertexAttribDivisor(5, 1);

    // --- Set up the static instance buffer ---
    glGenBuffers(1, &staticBatchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, staticBatchVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    // Cleanup: unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  bool Renderer2D::cannotBeRendered(const std::shared_ptr<SpriteRenderer> &r) {
    return !r || !r->Entity()->IsActive() || !r->Transform()->GetIsVisible() || !r->shader || !r->sprite
           || !r->sprite->texture;
  }

  bool Renderer2D::sortRenderers(const std::shared_ptr<SpriteRenderer> &a, const std::shared_ptr<SpriteRenderer> &b) {
    if (const bool aValid = cannotBeRendered(a); aValid != cannotBeRendered(b))
      return !aValid;

    if (a->shader->id != b->shader->id)
      return a->shader->id < b->shader->id;
    return a->sprite->texture->id < b->sprite->texture->id;
  }

  void Renderer2D::extractRendererData(
    const std::shared_ptr<SpriteRenderer> &renderer, float *data
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    if (const auto entity = renderer->Entity(); !renderer->Entity()->IsActive() || !renderer->Transform()->GetIsVisible())
      return;

    // Get the model matrix
    auto modelMatrix = renderer->Transform()->GetWorldMatrix();

    // Add scale and rotation
    *data++ = modelMatrix[0][0];
    *data++ = modelMatrix[0][1];
    *data++ = modelMatrix[1][0];
    *data++ = modelMatrix[1][1];

    // Add position and pivot
    *data++ = modelMatrix[3][0];
    *data++ = modelMatrix[3][1];
    *data++ = renderer->sprite->pivot.x;
    *data++ = renderer->sprite->pivot.y;

    // Add color
    *data++ = renderer->color.x;
    *data++ = renderer->color.y;
    *data++ = renderer->color.z;
    *data++ = renderer->color.w;

    // Add rect
    *data++ = renderer->sprite->rect.x;
    *data++ = renderer->sprite->rect.y;
    *data++ = renderer->sprite->rect.z;
    *data++ = renderer->sprite->rect.w;

    // Add renderOrder and pixelsPerUnit
    *data++ = renderer->renderOrder;
    *data++ = renderer->sprite->pixelsPerUnit;
    *data++ = 0;
    *data = 0;
  }

  void Renderer2D::updateStaticBatch() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    for (size_t i = 0; i < staticRenderers.size(); i++) {
      auto &renderer = staticRenderers[i];
      if (!renderer->dirty)
        continue;

      const size_t offset = i * STRIDE;
      extractRendererData(renderer, &staticBatchData[offset]);
      renderer->dirty = false;
    }
  }

  void Renderer2D::buildAndRenderStaticBatch(const bool rebuild) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (rebuild) {
      std::ranges::sort(staticRenderers, sortRenderers);

      staticBatchData.clear();
      staticFlushList.clear();

      uint32_t start = 0, count = 0, currentShaderID = 0, currentTextureID = 0, index = 0;

      // Only extract data for valid sprite renderers
      const auto it = std::ranges::find_if(staticRenderers, cannotBeRendered);
      const auto validRange = std::ranges::subrange(staticRenderers.begin(), it);
      staticBatchData.reserve(validRange.size() * STRIDE);
      staticBatchData.resize(validRange.size() * STRIDE);

      for (auto &renderer: validRange) {
        // Check for change in key
        const uint32_t shaderID = renderer->shader->id;
        if (const uint32_t textureID = renderer->sprite->texture->id;
          shaderID != currentShaderID || textureID != currentTextureID) {
          // Save the previous flush range if any
          if (count > 0) {
            staticFlushList.emplace_back(currentShaderID, currentTextureID, start, count);
            start += count;
            count = 0;
          }

          currentShaderID = shaderID;
          currentTextureID = textureID;
        }

        extractRendererData(renderer, &staticBatchData[index * STRIDE]);
        count++;
        index++;
      }
      if (count > 0)
        staticFlushList.emplace_back(currentShaderID, currentTextureID, start, count);
    }

    updateStaticBatch();

    for (auto &[shaderID, textureID, start, count]: staticFlushList) {
      if (shaderID != lastShaderID) {
        lastShaderID = shaderID;
        const auto shader = Engine::ResourceManager::GetShaderById(shaderID);
        shader->use();
        shader->SetMatrix4("projection", Game2D::MainCamera()->GetViewProjectionMatrix());
      }
      if (textureID != lastTextureID) {
        lastTextureID = textureID;
        Engine::ResourceManager::GetTexture2DById(textureID)->bind();
      }

      flush(staticBatchVBO, &staticBatchData[start * STRIDE], GL_STATIC_DRAW, count);
    }
  }

  void Renderer2D::buildAndRenderDynamicBatch() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    const auto it = std::ranges::find_if(renderers, cannotBeRendered);
    const auto validRange = std::ranges::subrange(renderers.begin(), it);

    batchData.resize(validRange.size() * STRIDE);
    for (const auto &renderer: validRange) {
      // Bind a new shader if needed
      if (lastShaderID != renderer->shader->id && renderer->shader->id > 0) {
        flush(batchVBO, &batchData[0], GL_DYNAMIC_DRAW, instanceCount);
        batchData.clear();
        instanceCount = 0;
        renderer->shader->use();
        lastShaderID = renderer->shader->id;
      }

      // Bind a new texture if needed
      if (lastTextureID != renderer->sprite->texture->id && renderer->sprite->texture->id > 0) {
        flush(batchVBO, &batchData[0], GL_DYNAMIC_DRAW, instanceCount);
        batchData.clear();
        instanceCount = 0;
        renderer->sprite->texture->bind();
        lastTextureID = renderer->sprite->texture->id;
      }

      if (instanceCount >= MAX_INSTANCE_COUNT || batchData.size() >= MAX_BATCH_SIZE_WITH_STRIDE) {
        flush(batchVBO, &batchData[0], GL_DYNAMIC_DRAW, instanceCount);
        batchData.clear();
        instanceCount = 0;
      }

      extractRendererData(renderer, &batchData[instanceCount * STRIDE]);
      instanceCount++;
    }

    // Draw any leftover sprites
    if (instanceCount > 0) {
      flush(batchVBO, &batchData[0], GL_DYNAMIC_DRAW, instanceCount);
      batchData.clear();
      instanceCount = 0;
    }
  }

  void Renderer2D::render() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    const size_t lastStaticCount = staticRenderers.size();
    const bool sort = !renderersToAdd.empty();

    // Add all pending renderers
    for (auto &renderer: renderersToAdd)
      if (renderer->Entity()->IsStatic())
        staticRenderers.emplace_back(renderer);
      else
        renderers.emplace_back(renderer);
    renderersToAdd.clear();

    // Remove all pending renderers
    for (auto &renderer: renderersToRemove)
      if (renderer->Entity()->IsStatic())
        std::erase(staticRenderers, renderer);
      else
        std::erase(renderers, renderer);
    renderersToRemove.clear();

    if (renderers.empty() && staticRenderers.empty())
      return;

    if (sort)
      std::ranges::sort(renderers, sortRenderers);

    if (!quadVAO)
      initRenderData();

    lastShaderID = 0;
    lastTextureID = 0;

    glBindVertexArray(quadVAO);
    buildAndRenderStaticBatch(lastStaticCount != staticRenderers.size());
    buildAndRenderDynamicBatch();
    glBindVertexArray(0);
  }

  void Renderer2D::flush(const uint VBO, const float *data, const int drawMode, const uint32_t count) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    if (!count || !data)
      return;

    // Update the instance VBO with the new data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * STRIDE_SIZE, data, drawMode);

    // Reset all the vertex attribute pointers
    constexpr GLsizei vec4Size = 4 * sizeof(float);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, nullptr);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(vec4Size));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(2 * vec4Size));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(3 * vec4Size));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(4 * vec4Size));

    // Draw all sprites in the batch in a single call.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, count);
  }

  void Renderer2D::addRenderer(const std::shared_ptr<SpriteRenderer> &renderer) {
    renderersToAdd.emplace_back(renderer);
  }

  void Renderer2D::removeRenderer(const std::shared_ptr<SpriteRenderer> &renderer) {
    renderersToRemove.emplace_back(renderer);
  }
}
