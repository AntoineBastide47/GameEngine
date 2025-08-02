//
// ParticleSystem2D.h
// Author: Antoine Bastide
// Date: 14/01/2025
//

#ifndef PARTICLE_SYSTEM2D_H
#define PARTICLE_SYSTEM2D_H

#include <glm/glm.hpp>

#include "Engine2D/Rendering/Renderable2D.hpp"
#include "ParticleSystem2D.gen.hpp"

namespace Engine::Rendering {
  class Shader;
}

using Engine::Rendering::Shader;

namespace Engine2D {
  namespace Rendering {
    class Sprite;
    class Renderer2D;
  }

  class ParticleSystem2D final : public Rendering::Renderable2D {
    SERIALIZE_PARTICLESYSTEM2D
      friend class Entity2D;
      friend class Rendering::Renderer2D;
      friend class ParticleSystemRegistry2D;
    public:
      enum BlendMode {
        Alpha, Additive, Multiply, PremultipliedAlpha, Subtractive, SoftAdditive, Opaque
      };

      /// If the particle system restarts after it finished simulating itself
      bool loop;
      /// If the particle system simulation should be restarted from the beginning
      bool restart;
      /// Whether particles will have their own start and end velocities
      bool useGlobalVelocities;
      /// If the simulation is run in world or local space
      bool simulateInWorldSpace;

      /// How long to wait before running the first simulation
      float startDelay;
      /// The initial position of the particles
      glm::vec2 startPosition;

      /// The initial velocity of the particles
      glm::vec2 startVelocity;
      /// The final velocity of the particles
      glm::vec2 endVelocity;
      /// The initial angular velocity of the particles
      float startAngularVelocity;
      /// The final angular velocity of the particles
      float endAngularVelocity;
      /// The initial scale of the particles
      glm::vec2 startScale;
      /// The scale of the particles at the end of there life
      glm::vec2 endScale;
      /// The initial color of the particles
      glm::vec4 startColor;
      /// The final color of the particles
      glm::vec4 endColor;

      /// How fast the simulation is run, 1.0f runs at normal sped
      float simulationSpeed;
      /// How many particles are simulated per second, a good value for a fluid simulation is maxParticles / simulationSpeed;
      float emissionRate;
      /// The maximum value that will randomly be added/removed from the particles start position
      float maxStartPositionOffset;
      /// How particles are blended on screen during rendering
      BlendMode blendMode;

      /// Sets how long the particle system simulation will last
      void SetDuration(float duration);
      /// @returns How long the simulation of particles lasts
      [[nodiscard]] float GetDuration() const;
      /// Sets the maximum number of particles this particle system can simulate at once
      /// @note Once the particle system has been updated once, this method will not do anything
      void SetMaxParticles(size_t maxParticles, bool uniformEmissionRate = true);
      /// @returns The maximum of particles this particle system will simulate
      [[nodiscard]] uint32_t GetMaxParticles() const;
      /// Sets how long each particle will live for
      void SetParticleLifetime(float lifetime);
      /// @returns how long each particle lives for
      [[nodiscard]] float GetParticleLifetime() const;

      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    protected:
      ParticleSystem2D();
      void forward() override;
      void recall() override;
    private:
      /// Represents a single particle and it's state
      struct Particle {
        glm::vec2 position;
        glm::vec2 scale;
        glm::vec2 startVelocity;
        glm::vec2 endVelocity;
        float rotation;
        float lifeTime;
      };

      /// How long the particle system will be simulated
      ENGINE_SERIALIZE float duration;
      /// How long particles will be visible on screen
      ENGINE_SERIALIZE float particleLifetime;
      /// The inverse of how long particles will be visible on screen
      float inverseLifetime;

      /// All the simulated particles
      std::vector<Particle> particles;
      /// The index of the first alive particle
      size_t head;
      /// How many particles are currently alive
      size_t capacity;
      /// The maximum number of particles that can be rendered
      ENGINE_SERIALIZE size_t maxParticles;
      /// Whether the simulation is done or not
      ENGINE_SERIALIZE bool simulationFinished;

      /// Emission accumulator timer
      float emissionAcc;
      /// Duration accumulator timer
      float durationAcc;

      /// The VAO used to render the particles
      uint quadVAO;
      /// The VBO used to render the particles
      uint quadVBO;
      /// The VBO used to send particle data
      uint instanceVBO;

      /// Renders the particle system
      void updateAndRender(uint textureIndex, float *data);
      /// Updates the dead particle at the given index and brings it back to life
      void respawnParticle();
  };
}

#endif //PARTICLE_SYSTEM2D_H
