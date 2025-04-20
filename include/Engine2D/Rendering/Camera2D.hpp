//
// Camera2D.hpp
// Author: Antoine Bastide
// Date: 20/04/2025
//

#ifndef CAMERA2D_HPP
#define CAMERA2D_HPP

#include <glm/glm.hpp>

#include "Engine2D/Component2D.hpp"

namespace Engine2D::Rendering {
  class Camera2D : public Component2D {
    friend class Renderer2D;
    public:
      Camera2D(float left, float right, float bottom, float top, float near = -1, float far = 1);
      ~Camera2D() override = default;

      const glm::mat4 &GetViewMatrix() const;
      const glm::mat4 &GetProjectionMatrix() const;
      const glm::mat4 &GetViewProjectionMatrix() const;
    private:
      glm::mat4 projection;
      glm::mat4 view;
      glm::mat4 viewProjection;

      void computeViewMatrix();
  };
}

#endif //CAMERA2D_HPP
