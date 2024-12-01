//
// ShapeRenderer.h
// Author: Antoine Bastide
// Date: 01/12/2024
//

#ifndef SHAPE_RENDERER_H
#define SHAPE_RENDERER_H

#include <vector>

#include "2D/Types/Vector2.h"

namespace Engine2D::Rendering {
  class Shader;
  /**
   * A class responsible for rendering 2D shapes (rectangles, circles, polygons) using OpenGL.
   * Provides methods to draw shapes in wireframe, fill, and fill with border modes.
   */
  class ShapeRenderer {
    public:
      /**
       * Constructs a ShapeRenderer with a specified shader.
       *
       * @param shader Pointer to a Shader object used for rendering the shapes.
       */
      explicit ShapeRenderer(Shader *shader);

      /** Cleans up any allocated OpenGL resources. */
      ~ShapeRenderer();

      // Rectangle drawing methods
      void DrawRectangleWireframe(const Vector2 &position, const Vector2 &size, const glm::vec3 &color) const;
      void DrawRectangleFill(const Vector2 &position, const Vector2 &size, const glm::vec3 &color) const;
      void DrawRectangleFillWithBorder(
        const Vector2 &position, const Vector2 &size, const glm::vec3 &fillColor, const glm::vec3 &borderColor,
        float borderWidth
      ) const;

      // Circle drawing methods
      void DrawCircleWireframe(const Vector2 &center, float radius, const glm::vec3 &color, int segments = 32) const;
      void DrawCircleFill(const Vector2 &center, float radius, const glm::vec3 &color, int segments = 32) const;
      void DrawCircleFillWithBorder(
        const Vector2 &center, float radius, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth,
        int segments = 32
      ) const;

      // Polygon drawing methods
      void DrawPolygonWireframe(const std::vector<Vector2> &vertices, const glm::vec3 &color) const;
      void DrawPolygonFill(const std::vector<Vector2> &vertices, const glm::vec3 &color) const;
      void DrawPolygonFillWithBorder(
        const std::vector<Vector2> &vertices, const glm::vec3 &fillColor, const glm::vec3 &borderColor, float borderWidth
      ) const;
    private:
      /** Shader that controls the rendering pipeline for the shapes. */
      Shader *shader;

      /** Stores the vertex configuration for the rectangle. */
      unsigned int rectangleVAO, rectangleVBO;

      /**
       * Initializes the vertex data and buffers for rendering shapes.
       * This setup is performed once in the constructor and used throughout the object's lifetime.
       */
      void initRenderData();
  };
}

#endif //SHAPE_RENDERER_H
