//
// Camera2D.hpp
// Author: Antoine Bastide
// Date: 20/04/2025
//

#ifndef CAMERA2D_HPP
#define CAMERA2D_HPP

#include <glm/glm.hpp>

#include "Engine2D/Component2D.hpp"
#include "Camera2D.gen.hpp"

namespace Engine2D {
  class Game2D;
}

namespace Engine {
  class ResourceManager;
}

namespace Engine2D::Rendering {
  class Camera2D final : public Component2D {
    SERIALIZE_CAMERA2D
      friend class Engine2D::Game2D;
      friend class Engine::ResourceManager;
    public:
      struct ShakeWave : Reflectable {
        SERIALIZE_SHAKEWAVE
          float amplitude;
          float frequency;
          float phase;

          ShakeWave()
            : amplitude(1.0f), frequency(1.0f), phase(0.0f) {}

          ShakeWave(const float amplitude, const float frequency, const float phase)
            : amplitude(amplitude), frequency(frequency), phase(phase) {}
      };

      /// The entity the camera follows
      Entity2D *followTarget;
      /// The offset position of the camera
      glm::vec2 positionOffset;
      /// The offset rotation of the camera
      float rotationOffset;
      /// Controls the smoothness of camera movement when following a target.
      /// Lower values result in faster, snappier motion. Higher values produce slower, smoother tracking.
      /// Must be greater than zero to avoid invalid calculations in smoothDamp.
      float damping;

      /// Coefficients that control the shake of the camera on the X axis
      std::vector<ShakeWave> shakeCoefficientsX;
      /// Coefficients that control the shake of the camera on the Y axis
      std::vector<ShakeWave> shakeCoefficientsY;

      Camera2D();
      Camera2D(float left, float right, float bottom, float top, float near = -1, float far = 1);
      ~Camera2D() override = default;

      /// Starts the camera shake effect
      void Shake(float duration);
      /// Checks if the sprite is visible to the camera
      /// @param position the position of the sprite
      /// @param scale the scale of the sprite
      bool IsInViewport(const glm::vec2 &position, const glm::vec2 &scale) const;

      /// @returns The orthographic projection matrix that defines the camera's visible region in world space.
      const glm::mat4 &GetProjectionMatrix() const;
      /// @returns The view matrix representing the inverse of the camera's world transformation (position, rotation, scale).
      const glm::mat4 &GetViewMatrix() const;
      /// @returns The combined view-projection matrix used to transform world coordinates into clip space.
      const glm::mat4 &GetViewProjectionMatrix() const;
    private:
      /// The orthographic projection matrix that defines the camera's visible region in world space.
      const glm::mat4 projection;
      /// The view matrix representing the inverse of the camera's world transformation (position, rotation, scale).
      glm::mat4 view;
      /// The combined view-projection matrix used to transform world coordinates into clip space.
      glm::mat4 viewProjection;
      /// If the camera is currently shaking
      bool shaking;
      /// Whether the camera is initialized or not
      bool initialized;
      /// How long the camera has been shaking for
      float shakeElapsed;
      /// How long the camera should shake for
      float shakeDuration;
      /// UBO used to send data to all shaders
      uint ubo;

      float m00, m01, m03;
      float m10, m11, m13;

      static uint ENGINE_DATA_BINDING_PORT;

      void initialize();
      void updateCamera();
      glm::vec2 getCameraShake(float frac) const;
  };
}

#endif //CAMERA2D_HPP
