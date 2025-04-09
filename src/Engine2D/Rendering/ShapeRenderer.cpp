//
// ShapeRenderer.cpp
// Author: Antoine Bastide
// Date: 01/12/2024
//

#include "Engine2D/Rendering/ShapeRenderer.hpp"
#include "../../../include/Engine/Rendering/Shader.hpp"
#include "Engine/RenderingHeaders.hpp"

namespace Engine2D::Rendering {
  std::shared_ptr<Shader> ShapeRenderer::shader = nullptr;

  void ShapeRenderer::DrawCircleWireframe(
    const glm::vec2 center, const float radius, const glm::vec3 &color, const int segments
  ) {
    std::vector<float> vertices;
    for (int i = 0; i <= segments; ++i) {
      const float theta = M_PI * 2.0f * static_cast<float>(i) / static_cast<float>(segments);
      float x = radius * cosf(theta);
      float y = radius * sinf(theta);
      vertices.push_back(x);
      vertices.push_back(y);
    }

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    shader->Use();
    const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f));
    shader->SetMatrix4("model", model);
    shader->SetVector3f("color", color);

    glBindVertexArray(vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINE_LOOP, 0, segments + 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void ShapeRenderer::DrawCircleFill(
    const glm::vec2 center, const float radius, const glm::vec3 &color, const int segments
  ) {
    std::vector<float> vertices;

    // Center of the circle
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; ++i) {
      const float theta = M_PI * 2.0f * static_cast<float>(i) / static_cast<float>(segments);
      float x = radius * cosf(theta);
      float y = radius * sinf(theta);
      vertices.push_back(x);
      vertices.push_back(y);
    }

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    shader->Use();
    const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f));
    shader->SetMatrix4("model", model);
    shader->SetVector3f("color", color);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void ShapeRenderer::DrawCircleFillWithBorder(
    const glm::vec2 center, const float radius, const glm::vec3 &fillColor, const glm::vec3 &borderColor,
    const float borderWidth, const int segments
  ) {
    // Draw filled circle
    DrawCircleFill(center, radius, fillColor, segments);

    // Set line width for border
    glLineWidth(borderWidth);

    // Draw border
    DrawCircleWireframe(center, radius, borderColor, segments);

    // Reset line width to default
    glLineWidth(1.0f);
  }

  void ShapeRenderer::DrawPolygonWireframe(const std::vector<glm::vec2> &vertices, const glm::vec3 &color) {
    std::vector<float> data;
    for (const auto &vertex: vertices) {
      data.push_back(vertex.x);
      data.push_back(vertex.y);
    }

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size() * sizeof(float)), data.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    shader->Use();
    constexpr auto model = glm::mat4(1.0f);
    shader->SetMatrix4("model", model);
    shader->SetVector3f("color", color);

    glBindVertexArray(vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(0.5f * static_cast<float>(data.size())));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void ShapeRenderer::DrawPolygonFill(const std::vector<glm::vec2> &vertices, const glm::vec3 &color) {
    // Note: For concave polygons, you need to triangulate the polygon.
    std::vector<float> data;
    for (const auto &vertex: vertices) {
      data.push_back(vertex.x);
      data.push_back(vertex.y);
    }

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size() * sizeof(float)), data.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    shader->Use();
    constexpr auto model = glm::mat4(1.0f);
    shader->SetMatrix4("model", model);
    shader->SetVector3f("color", color);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(data.size()));
    glBindVertexArray(0);

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void ShapeRenderer::DrawPolygonFillWithBorder(
    const std::vector<glm::vec2> &vertices, const glm::vec3 &fillColor, const glm::vec3 &borderColor, const float borderWidth
  ) {
    // Draw filled polygon
    DrawPolygonFill(vertices, fillColor);

    // Set line width for border
    glLineWidth(borderWidth);

    // Draw border
    DrawPolygonWireframe(vertices, borderColor);

    // Reset line width to default
    glLineWidth(1.0f);
  }
}