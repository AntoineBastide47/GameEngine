//
// ShapeRenderer.h
// Author: Antoine Bastide
// Date: 01/12/2024
//

#ifndef SHAPE_RENDERER_H
#define SHAPE_RENDERER_H

#include <vector>

#include "2D/Types/Vector2.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Rendering {
  class Shader;
  /**
   * A class responsible for rendering 2D shapes (rectangles, circles, polygons) using OpenGL.
   * Provides methods to draw shapes in wireframe, fill, and fill with border modes.
   */
  class ShapeRenderer {
    friend class Engine2D::Game2D;
    public:
      // Rectangle drawing methods
      static void DrawRectangleWireframe(Vector2f position, Vector2f size, const glm::vec3 &color);
      static void DrawRectangleFill(Vector2f position, Vector2f size, const glm::vec3 &color);
      static void DrawRectangleFillWithBorder(
        Vector2f position, Vector2f size, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth
      );

      // Circle drawing methods
      static void DrawCircleWireframe(Vector2f center, float radius, const glm::vec3 &color, int segments = 32);
      static void DrawCircleFill(Vector2f center, float radius, const glm::vec3 &color, int segments = 32);
      static void DrawCircleFillWithBorder(
        Vector2f center, float radius, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth,
        int segments = 32
      );

      // Polygon drawing methods
      static void DrawPolygonWireframe(const std::vector<Vector2f> &vertices, const glm::vec3 &color);
      static void DrawPolygonFill(const std::vector<Vector2f> &vertices, const glm::vec3 &color);
      static void DrawPolygonFillWithBorder(
        const std::vector<Vector2f> &vertices, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth
      );
    private:
      /** Shader that controls the rendering pipeline for the shapes. */
      static std::shared_ptr<Shader> shader;
      /** Stores the vertex configuration for the rectangle. */
      static unsigned int rectangleVAO, rectangleVBO;

      /** Default constructor */
      ShapeRenderer() = default;
      /** Cleans up any allocated OpenGL resources. */
      ~ShapeRenderer();

      /**
       * Initializes the vertex data and buffers for rendering shapes.
       * This setup is performed once in the constructor and used throughout the object's lifetime.
       */
      static void initRenderData();
  };
}

#endif //SHAPE_RENDERER_H
