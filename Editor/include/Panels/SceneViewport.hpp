//
// SceneViewport.hpp
// Author: Antoine Bastide
// Date: 14.08.2025
//

#ifndef SCENE_VIEWPORT_HPP
#define SCENE_VIEWPORT_HPP

#include <cstdint>

namespace Editor {
  class SceneViewport final {
    friend class LevelEditor;
    public:
      /// Initializes the viewport
      static void Initialize();
      /// Renders the viewport of the level editor
      static void Render(const ImGuiWindowClass *winClass);
      /// Free's up all the cpu and gpu memory
      static void Cleanup();

      /// @returns True if the viewport is currently focused, False if not
      static bool Focused();
      /// @returns True if the viewport is currently hovered, False if not
      static bool Hovered();
    private:
      inline static uint32_t rendererID = 0, colorAttachment = 0, depthAttachment = 0;
      inline static uint32_t highResRendererID = 0, highResColorAttachment = 0, highResDepthAttachment = 0;
      inline static bool viewportFocused = false;
      inline static bool viewportHovered = false;
      inline static int maxFramebufferDims[2];

      struct FrameBufferData {
        uint32_t width, height;
        static constexpr float supersampleFactor = 2.0f;
      };

      inline static FrameBufferData frameBufferData{800, 600};

      /// Create's a framebuffer for the scene based on the given data
      static void createSceneFrameBuffer(const FrameBufferData &data);
      /// Bind's the scene's framebuffer
      static void bindSceneFrameBuffer();
      /// Unbind's the scene's framebuffer
      static void unbindSceneFrameBuffer();
      /// Resize's the scene's framebuffer to the given proportions
      static void resizeSceneFrameBuffer(uint32_t width, uint32_t height);
  };
} // Editor

#endif //SCENE_VIEWPORT_HPP
