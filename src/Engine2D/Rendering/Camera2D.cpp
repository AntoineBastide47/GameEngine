//
// Camera2D.cpp
// Author: Antoine Bastide
// Date: 20/04/2025
//

#include <glm/gtc/matrix_transform.hpp>

#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Transform2D.hpp"

namespace Engine2D::Rendering {
  Camera2D::Camera2D(
    const float left, const float right, const float bottom, const float top, const float near, const float far
  ) : projection(glm::ortho(left, right, bottom, top, near, far)), view(1.0f) {
    viewProjection = projection * view;
  }

  const glm::mat4 &Camera2D::GetViewMatrix() const {
    return this->view;
  }

  const glm::mat4 &Camera2D::GetProjectionMatrix() const {
    return this->projection;
  }

  const glm::mat4 &Camera2D::GetViewProjectionMatrix() const {
    return this->viewProjection;
  }

  void Camera2D::computeViewMatrix() {
    view = glm::inverse(Transform()->GetWorldMatrix());
    viewProjection = projection * view;
  }
}
