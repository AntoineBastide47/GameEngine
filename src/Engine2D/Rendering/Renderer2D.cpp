//
// SpriteRenderer.cpp
// Author: Antoine Bastide (modified for batching)
// Date: 10/11/2024 (modified 02/03/2025)
//

#include <vector>

#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Macros/Assert.hpp"
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
    0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
    1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 1.0f, 1.0f, 1.0f  // Top-right
  };

  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::opaqueRenderers;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::transparentRenderers;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::opaqueStaticRenderers;
  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::transparentStaticRenderers;

  std::vector<std::shared_ptr<SpriteRenderer>> Renderer2D::renderersToAdd;
  std::unordered_set<std::shared_ptr<SpriteRenderer>> Renderer2D::renderersToRemove;

  uint Renderer2D::quadVAO{};
  uint Renderer2D::batchVBO{};
  uint Renderer2D::staticBatchVBO{};
  uint Renderer2D::lastBoundTexture{};

  uint Renderer2D::instanceCount;
  uint Renderer2D::lastShaderID;
  uint Renderer2D::lastTextureID;
  size_t Renderer2D::lastOpaqueCount;
  size_t Renderer2D::lastTransparentCount;

  std::vector<float> Renderer2D::batchData;
  std::vector<float> Renderer2D::opaqueStaticBatchData;
  std::vector<float> Renderer2D::transparentStaticBatchData;
  std::vector<Flush> Renderer2D::opaqueStaticFlushList;
  std::vector<Flush> Renderer2D::transparentStaticFlushList;

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

    opaqueRenderers.clear();
    transparentRenderers.clear();
    batchData.clear();

    opaqueStaticRenderers.clear();
    transparentStaticRenderers.clear();
    opaqueStaticBatchData.clear();
    transparentStaticBatchData.clear();
    opaqueStaticFlushList.clear();
    transparentStaticFlushList.clear();
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
    return !r->IsActive() || !r->Entity()->IsActive() || !r->Transform()->GetIsVisible() || !r->shader || !r->sprite
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
    const std::shared_ptr<SpriteRenderer> &renderer, float *data, const bool transparent
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    if (const auto entity = renderer->Entity(); !renderer->Entity()->IsActive() || !renderer->IsActive()
                                                || !renderer->Transform()->GetIsVisible())
      return;

    // Get the model matrix and other data
    auto modelMatrix = renderer->Transform()->GetWorldMatrix();
    const auto sprite = renderer->sprite;
    const auto color = renderer->color;
    const auto rect = sprite->rect;

    // Add scale and rotation
    *data++ = modelMatrix[0][0];
    *data++ = modelMatrix[0][1];
    *data++ = modelMatrix[1][0];
    *data++ = modelMatrix[1][1];

    // Add position and pivot
    *data++ = modelMatrix[3][0];
    *data++ = modelMatrix[3][1];
    *data++ = sprite->pivot.x;
    *data++ = sprite->pivot.y;

    // Add color
    *data++ = color.x;
    *data++ = color.y;
    *data++ = color.z;
    *data++ = color.w;

    // Add rect
    *data++ = rect.x;
    *data++ = rect.y;
    *data++ = rect.z;
    *data++ = rect.w;

    // Add renderOrder and pixelsPerUnit
    *data++ = renderer->renderOrder;
    *data++ = sprite->pixelsPerUnit;
    *data++ = 0;
    *data = 0;
  }

  void Renderer2D::updateStaticBatch(
    const std::vector<std::shared_ptr<SpriteRenderer>> &renderers, std::vector<float> &staticBatchData,
    const bool transparent
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    for (size_t i = 0; i < renderers.size(); i++) {
      auto &renderer = renderers[i];
      if (!renderer || !renderer->dirty)
        continue;

      const size_t offset = i * STRIDE;
      extractRendererData(renderer, &staticBatchData[offset], transparent);
      renderer->dirty = false;
    }
  }

  void Renderer2D::buildAndRenderStaticBatch(
    std::vector<std::shared_ptr<SpriteRenderer>> &renderers, std::vector<float> &staticBatchData,
    std::vector<Flush> &staticFlushList, const bool transparent, const uint lastStaticCount
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (renderers.size() != lastStaticCount) {
      std::ranges::sort(renderers, sortRenderers);

      staticBatchData.clear();
      staticFlushList.clear();

      uint32_t start = 0, count = 0, currentShaderID = 0, currentTextureID = 0, index = 0;

      // Only extract data for valid sprite renderers
      const auto it = std::ranges::find_if(renderers, cannotBeRendered);
      const auto validRange = std::ranges::subrange(renderers.begin(), it);
      staticBatchData.reserve(validRange.size() * STRIDE);
      staticBatchData.resize(validRange.size() * STRIDE);

      for (auto &renderer: validRange) {
        // Check for change in shader id
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

        extractRendererData(renderer, &staticBatchData[index * STRIDE], transparent);
        count++;
        index++;
      }
      if (count > 0)
        staticFlushList.emplace_back(currentShaderID, currentTextureID, start, count);
    }

    updateStaticBatch(renderers, staticBatchData, transparent);

    for (auto &[shaderID, textureID, start, count]: staticFlushList) {
      if (shaderID != lastShaderID) {
        lastShaderID = shaderID;
        Engine::ResourceManager::GetShaderById(shaderID)->
          SetMatrix4("projection", Game2D::MainCamera()->GetViewProjectionMatrix(), true);
      }
      if (textureID != lastTextureID) {
        lastTextureID = textureID;
        Engine::ResourceManager::GetTexture2DById(textureID)->bind();
      }

      flush(staticBatchVBO, &staticBatchData[start * STRIDE], GL_STATIC_DRAW, count);
    }
  }

  void Renderer2D::buildAndRenderDynamicBatch(
    const std::vector<std::shared_ptr<SpriteRenderer>> &renderers, const bool transparent
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    const auto it = std::ranges::find_if(renderers, cannotBeRendered);
    const auto validRange = std::ranges::subrange(renderers.begin(), it);

    batchData.clear();
    batchData.resize(validRange.size() * STRIDE);
    for (const auto &renderer: validRange) {
      // Bind a new shader if needed
      if (lastShaderID != renderer->shader->id && renderer->shader->id > 0) {
        flush(batchVBO, &batchData[0], GL_DYNAMIC_DRAW, instanceCount);
        batchData.clear();
        instanceCount = 0;
        renderer->shader->SetMatrix4("projection", Game2D::MainCamera()->GetViewProjectionMatrix(), true);
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

      extractRendererData(renderer, &batchData[instanceCount * STRIDE], transparent);
      instanceCount++;
    }

    // Draw any leftover sprites
    if (instanceCount > 0) {
      flush(batchVBO, &batchData[0], GL_DYNAMIC_DRAW, instanceCount);
      batchData.clear();
      instanceCount = 0;
    }
  }

  void Renderer2D::prerender() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    lastOpaqueCount = opaqueStaticRenderers.size();
    lastTransparentCount = transparentStaticRenderers.size();
    const bool sort = !renderersToAdd.empty();

    // Add all pending renderers
    if (!renderersToAdd.empty()) {
      for (const auto &renderer: renderersToAdd)
        ENGINE_ASSERT(renderer->sprite, "SpriteRenderer missing a sprite");

      // First partition: static vs. non-static
      auto static_end = std::ranges::stable_partition(
        renderersToAdd, [](const auto &renderer) {
          return renderer->Entity()->IsStatic();
        }
      );

      // Second partition within static: opaque vs. transparent
      auto static_transparent_end = std::ranges::stable_partition(
        renderersToAdd.begin(), static_end.begin(), [](const auto &renderer) {
          return renderer->sprite->transparent;
        }
      );

      // Third partition within non-static: opaque vs. transparent
      auto dynamic_transparent_end = std::ranges::stable_partition(
        static_end.begin(), renderersToAdd.end(), [](const auto &renderer) {
          return renderer->sprite->transparent;
        }
      );

      transparentStaticRenderers.insert(
        transparentStaticRenderers.end(), renderersToAdd.begin(), static_transparent_end.begin()
      );
      opaqueStaticRenderers.insert(
        opaqueStaticRenderers.end(), static_transparent_end.begin(), static_transparent_end.end()
      );
      transparentRenderers.insert(transparentRenderers.end(), static_transparent_end.end(), dynamic_transparent_end.begin());
      opaqueRenderers.insert(
        opaqueRenderers.end(), dynamic_transparent_end.begin(), dynamic_transparent_end.end()
      );

      renderersToAdd.clear();
    }

    // Remove all pending renderers
    if (!renderersToRemove.empty()) {
      std::erase_if(
        opaqueStaticRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
      std::erase_if(
        transparentStaticRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
      std::erase_if(
        transparentRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
      std::erase_if(
        opaqueRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );

      renderersToRemove.clear();
    }

    if (sort) {
      std::ranges::sort(opaqueRenderers, sortRenderers);
      std::ranges::sort(transparentRenderers, sortRenderers);
    }
  }

  void Renderer2D::render(const bool renderTransparent) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (opaqueRenderers.empty() && transparentRenderers.empty() && opaqueStaticRenderers.empty() &&
        transparentStaticRenderers.empty())
      return;

    if (!quadVAO)
      initRenderData();

    lastShaderID = 0;
    lastTextureID = 0;

    glBindVertexArray(quadVAO);
    if (!renderTransparent) {
      buildAndRenderStaticBatch(opaqueStaticRenderers, opaqueStaticBatchData, opaqueStaticFlushList, false, lastOpaqueCount);
      buildAndRenderDynamicBatch(opaqueRenderers, false);
    } else {
      // Reset the depth buffer
      glClear(GL_DEPTH_BUFFER_BIT);
      buildAndRenderStaticBatch(
        transparentStaticRenderers, transparentStaticBatchData, transparentStaticFlushList, true, lastTransparentCount
      );
      buildAndRenderDynamicBatch(transparentRenderers, true);
    }
    glBindVertexArray(0);
  }

  void Renderer2D::flush(
    const uint VBO, const float *data, const int drawMode, const uint32_t count, const uint32_t framebuffer
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    if (!count || !data)
      return;

    // Bind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

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
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);

    // Unbind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Renderer2D::addRenderer(const std::shared_ptr<SpriteRenderer> &renderer) {
    renderersToAdd.emplace_back(renderer);
  }

  void Renderer2D::removeRenderer(const std::shared_ptr<SpriteRenderer> &renderer) {
    renderersToRemove.insert(renderer);
  }
}
