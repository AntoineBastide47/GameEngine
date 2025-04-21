//
// ParticleSystem2D.h
// Author: Antoine Bastide
// Date: 14/01/2025
//

#ifndef PARTICLE_SYSTEM2D_H
#define PARTICLE_SYSTEM2D_H

#include <random>

#include "Engine2D/Component2D.hpp"

namespace Engine::Rendering {
  class Shader;
}

using Engine::Rendering::Shader;

namespace Engine2D {
  namespace Rendering {
    class Sprite;
  }
  class ParticleSystem2D final : public Component2D {
    friend class Game2D;
    friend class Entity2D;
    friend class ParticleSystemRenderer2D;
    public:
      /// If the particle system restarts after it finished simulating itself
      bool loop;
      /// If the particle system simulation should be restarted from the beginning
      bool restart;

      /// How long to wait before running the first simulation
      float startDelay;
      /// How long particles will be visible on screen
      float particleLifetime;
      /// The initial position of the particles
      glm::vec2 startPosition;
      /// Rendering priority: higher value means higher priority
      int16_t renderOrder;

      /// Whether particles will have their own start and end velocities
      bool useGlobalVelocities;
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

      /// If the simulation is run in world or local space
      bool simulateInWorldSpace;
      /// How fast the simulation is run, 1.0f runs at normal sped
      float simulationSpeed;
      /// How many particles are simulated per second, a good value for a fluid simulation is maxParticles / simulationSpeed;
      float emissionRate;
      /// The maximum value that will randomly be added/removed from the particles start position
      float maxStartPositionOffset;

      /// The shader of used to render the particles, defaults to the "particle" shader
      std::shared_ptr<Shader> shader;
      /// The sprite used to render the particles
      std::shared_ptr<Rendering::Sprite> sprite;

      ParticleSystem2D();

      /// Sets how long the particle system simulation will last
      void SetDuration(float duration);
      /// @returns How long the simulation of particles lasts
      [[nodiscard]] float GetDuration() const;
      /// Sets the maximum number of particles this particle system can simulate at once
      /// @note Once the particle system has been updated once, this method will not do anything
      void SetMaxParticles(size_t maxParticles);
      /// @returns The maximum of particles this particle system will simulate
      [[nodiscard]] uint32_t GetMaxParticles() const;
    private:
      /// Represents a single particle and it's state
      struct Particle {
        /// The position of the particle
        glm::vec2 position;
        /// The rotation of the particle
        float rotation;
        /// The remaining life span of the particle
        float lifeTime;
        /// The scale of the particle
        glm::vec2 scale;
        /// The color of the particle
        glm::vec4 color;
        /// The initial velocity of the particles
        glm::vec2 startVelocity;
        /// The final velocity of the particles
        glm::vec2 endVelocity;

        bool visible;
        bool Visible();
      };

      /// How long the particle system will be simulated
      float duration;

      /// The list of all the simulated particles
      std::vector<Particle> particles;
      /// Emission accumulator timer
      float emissionAcc;
      /// Duration accumulator timer
      float durationAcc;

      /// Whether the simulation is done or not
      bool simulationFinished;
      /// The index of the last dead particle in the list
      uint lastUsedParticle;

      /// The VAO used to render the particles
      uint quadVAO;
      /// The VBO used to render the particles
      uint quadVBO;
      /// The VBO used to send particle data
      uint instanceVBO;
      /// How many particles are alive
      uint aliveCount;
      /// Whether this component has been initialized or not
      bool initialized;

      /// The vertices used to define the quad used to render the particles
      static const float quadVertices[];

      /// Initializes the particle system
      void initialize();
      /// Checks whether the current particle is in a valid state
      bool validParticle(const Particle &p) const;
      /// Updates all the particles
      void update();
      /// Renders the particle system
      void render();

      /// Updates the dead particle at the given index and brings it back to life
      void respawnParticle(uint index);
  };
}

#endif //PARTICLE_SYSTEM2D_H
