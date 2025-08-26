//
// SceneViewport.cpp
// Author: Antoine Bastide
// Date: 14.08.2025
//

#include <imgui.h>

#include "Panels/SceneViewport.hpp"

#include <Engine/Macros/Assert.hpp>

#include "LevelEditor.hpp"
#include "ProjectLoader.hpp"
#include "Engine/RenderingHeaders.hpp"

namespace Editor {
  void SceneViewport::Initialize() {
    int size[2];
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, size);
    maxFramebufferDims[0] = size[0];
    maxFramebufferDims[1] = size[1];
    createSceneFrameBuffer(frameBufferData);
  }

  void SceneViewport::Render(const ImGuiWindowClass *winClass) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowClass(winClass);
    ImGui::Begin("Viewport");
    const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    // Ensure minimum viewport size
    const float width = std::max(viewportSize.x, 800.0f);
    const float height = std::max(viewportSize.y, 600.0f);

    if ((frameBufferData.width != width || frameBufferData.height != height) && width > 0 && height > 0)
      resizeSceneFrameBuffer(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    bindSceneFrameBuffer();
    if (ProjectLoader::IsProjectLoaded() && LevelEditor::loadedGame) {
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      LevelEditor::loadedGame->renderFrame();
    } else if (ProjectLoader::IsProjectLoaded()) {
      glClearColor(0.3f, 0.3f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    unbindSceneFrameBuffer();

    // Cast texture ID for ImGui (flipped on Y-axis)
    ImGui::Image(colorAttachment, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

    // Update viewport state for input processing
    viewportFocused = ImGui::IsWindowFocused();
    viewportHovered = ImGui::IsItemHovered();
    ImGui::End();
    ImGui::PopStyleVar();
  }

  void SceneViewport::Cleanup() {
    // Clean up display resolution framebuffer
    if (rendererID) {
      glDeleteFramebuffers(1, &rendererID);
      rendererID = 0;
    }
    if (colorAttachment) {
      glDeleteTextures(1, &colorAttachment);
      colorAttachment = 0;
    }
    if (depthAttachment) {
      glDeleteTextures(1, &depthAttachment);
      depthAttachment = 0;
    }

    // Clean up high resolution framebuffer
    if (highResRendererID) {
      glDeleteFramebuffers(1, &highResRendererID);
      highResRendererID = 0;
    }
    if (highResColorAttachment) {
      glDeleteTextures(1, &highResColorAttachment);
      highResColorAttachment = 0;
    }
    if (highResDepthAttachment) {
      glDeleteTextures(1, &highResDepthAttachment);
      highResDepthAttachment = 0;
    }
  }

  bool SceneViewport::Focused() {
    return viewportFocused;
  }

  bool SceneViewport::Hovered() {
    return viewportHovered;
  }

  void SceneViewport::createSceneFrameBuffer(const FrameBufferData &data) {
    if (rendererID)
      Cleanup();

    const uint32_t highResWidth = static_cast<uint32_t>(data.width * data.supersampleFactor);
    const uint32_t highResHeight = static_cast<uint32_t>(data.height * data.supersampleFactor);

    // Create high-resolution framebuffer for supersampling
    glGenFramebuffers(1, &highResRendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, highResRendererID);

    glGenTextures(1, &highResColorAttachment);
    glBindTexture(GL_TEXTURE_2D, highResColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, highResWidth, highResHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, highResColorAttachment, 0);

    glGenTextures(1, &highResDepthAttachment);
    glBindTexture(GL_TEXTURE_2D, highResDepthAttachment);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, highResWidth, highResHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
      nullptr
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, highResDepthAttachment, 0);

    ENGINE_ASSERT(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "High-res framebuffer incomplete"
    );

    // Create display-resolution framebuffer
    glGenFramebuffers(1, &rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    glGenTextures(1, &depthAttachment);
    glBindTexture(GL_TEXTURE_2D, depthAttachment);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, data.width, data.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

    ENGINE_ASSERT(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Display framebuffer incomplete"
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void SceneViewport::bindSceneFrameBuffer() {
    const uint32_t highResWidth = static_cast<uint32_t>(frameBufferData.width * frameBufferData.supersampleFactor);
    const uint32_t highResHeight = static_cast<uint32_t>(frameBufferData.height * frameBufferData.supersampleFactor);

    glBindFramebuffer(GL_FRAMEBUFFER, highResRendererID);
    glViewport(0, 0, highResWidth, highResHeight);
  }

  void SceneViewport::unbindSceneFrameBuffer() {
    // Blit high-resolution framebuffer to display-resolution framebuffer with linear filtering
    glBindFramebuffer(GL_READ_FRAMEBUFFER, highResRendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rendererID);

    const uint32_t highResWidth = static_cast<uint32_t>(frameBufferData.width * frameBufferData.supersampleFactor);
    const uint32_t highResHeight = static_cast<uint32_t>(frameBufferData.height * frameBufferData.supersampleFactor);

    glBlitFramebuffer(
      0, 0, highResWidth, highResHeight,
      0, 0, frameBufferData.width, frameBufferData.height,
      GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void SceneViewport::resizeSceneFrameBuffer(const uint32_t width, const uint32_t height) {
    if (width == 0 || height == 0 || width > maxFramebufferDims[0] || height > maxFramebufferDims[1])
      return;

    frameBufferData.width = width;
    frameBufferData.height = height;
    createSceneFrameBuffer(frameBufferData);

    if (LevelEditor::loadedGame)
      LevelEditor::loadedGame->framebuffer_size_callback(nullptr, width, height);
  }
} // Editor
