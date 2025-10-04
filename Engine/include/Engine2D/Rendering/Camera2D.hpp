//
// Camera2D.hpp
// Author: Antoine Bastide
// Date: 20/04/2025
//

#ifndef CAMERA2D_HPP
#define CAMERA2D_HPP

#include <glm/glm.hpp>

#include "Engine/Reflection/ICustomEditor.hpp"
#include "Engine/Types/Ptr.hpp"
#include "Engine2D/Component2D.hpp"
#include "Camera2D.gen.hpp"

namespace Engine2D {
  class Scene;
  class Game2D;
  class Entity2D;
}

namespace Engine {
  class ResourceManager;
}

namespace Engine2D::Rendering {
  class Camera2D final : public Component2D, public Engine::Reflection::ICustomEditor {
    SERIALIZE_CAMERA2D
      friend class Engine2D::Scene;
      friend class Engine2D::Game2D;
      friend class Engine2D::Entity2D;
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
      Engine::Ptr<Entity2D> followTarget;
      /// The offset position of the camera
      glm::vec2 positionOffset;
      /// The offset rotation of the camera
      float rotationOffset;
      /// Controls the smoothness of camera movement when following a target.
      /// Lower values result in faster, snappier motion. Higher values produce slower, smoother tracking.
      /// Must be greater than zero to avoid invalid calculations in smoothDamp.
      float damping;
      /// How zoomed in the camera is
      float zoomLevel;

      /// Coefficients that control the shake of the camera on the X axis
      std::vector<ShakeWave> shakeCoefficientsX;
      /// Coefficients that control the shake of the camera on the Y axis
      std::vector<ShakeWave> shakeCoefficientsY;

      ~Camera2D() override;

      /// Starts the camera shake effect
      void Shake(float duration);
      /// Checks if the sprite is visible to the camera
      /// @param position the position of the sprite
      /// @param scale the scale of the sprite
      bool IsInViewport(const glm::vec2 &position, const glm::vec2 &scale) const;

      void Resize(float width, float height);
      void SetProjection(float left, float right, float bottom, float top);

      /// @returns The orthographic projection matrix that defines the camera's visible region in world space.
      const glm::mat4 &ProjectionMatrix() const;
      /// @returns The view matrix representing the inverse of the camera's world transformation (position, rotation, scale).
      const glm::mat4 &ViewMatrix() const;
      /// @returns The combined view-projection matrix used to transform world coordinates into clip space.
      const glm::mat4 &ViewProjectionMatrix() const;

      void OnSerialize(Engine::Reflection::Format format, Engine::JSON &json) const override;
      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    private:
      /// The orthographic projection matrix that defines the camera's visible region in world space.
      ENGINE_SERIALIZE_HIDDEN glm::mat4 projection;
      /// The view matrix representing the inverse of the camera's world transformation (position, rotation, scale).
      glm::mat4 view;
      /// The combined view-projection matrix used to transform world coordinates into clip space.
      glm::mat4 viewProjection;
      /// Whether the camera is initialized or not
      bool initialized;
      /// If the camera is currently shaking
      bool shaking;
      /// How long the camera should shake for
      ENGINE_SERIALIZE float shakeDuration;
      /// How long the camera has been shaking for
      float shakeElapsed;
      /// UBO used to send data to all shaders
      uint ubo;
      /// The index of the entity in the entities vector the camera is following
      int followTargetIndex;
      /// The velocity of the follow camera velocity
      glm::vec2 velocity;
      glm::mat2 rotationMatrix;
      float worldRotationLastFrame;
      float invZoom;

      ENGINE_SERIALIZE_HIDDEN float left, right, bottom, top;

      inline static uint ENGINE_DATA_BINDING_PORT = 0;
      static constexpr float near = -32768.0f;
      static constexpr float far = 32768.0f;

      Camera2D();
      Camera2D(float left, float right, float bottom, float top);

      void initialize();
      void updateCamera();
      glm::vec2 getCameraShake(float frac) const;

      #if ENGINE_EDITOR
      bool OnRenderInEditor(const std::string &name, bool isConst, bool readOnly) override;
      #endif
  };
}

#endif //CAMERA2D_HPP
