//
// ShapeRenderer.h
// Author: Antoine Bastide
// Date: 01/12/2024
//

#ifndef SHAPE_RENDERER_H
#define SHAPE_RENDERER_H

#include <vector>
#include <glm/glm.hpp>

namespace Engine2D {
  class Game2D;
}
namespace Engine::Rendering {
  class Shader;
}

using Engine::Rendering::Shader;

namespace Engine2D::Rendering {
  /**
   * A class responsible for rendering 2D shapes (rectangles, circles, polygons) using OpenGL.
   * Provides methods to draw shapes in wireframe, fill, and fill with border modes.
   */
  class ShapeRenderer {
    friend class Engine2D::Game2D;
    public:
      // Circle drawing methods
      static void DrawCircleWireframe(glm::vec2 center, float radius, const glm::vec3 &color, int segments = 32);
      static void DrawCircleFill(glm::vec2 center, float radius, const glm::vec3 &color, int segments = 32);
      static void DrawCircleFillWithBorder(
        glm::vec2 center, float radius, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth,
        int segments = 32
      );

      // Polygon drawing methods
      static void DrawPolygonWireframe(const std::vector<glm::vec2> &vertices, const glm::vec3 &color);
      static void DrawPolygonFill(const std::vector<glm::vec2> &vertices, const glm::vec3 &color);
      static void DrawPolygonFillWithBorder(
        const std::vector<glm::vec2> &vertices, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth
      );
    private:
      /** Shader that controls the rendering pipeline for the shapes. */
      static std::shared_ptr<Shader> shader;

      /** Default constructor */
      ShapeRenderer() = default;
  };
}

#endif //SHAPE_RENDERER_H
