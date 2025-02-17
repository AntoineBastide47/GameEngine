//
// ParticleSystem2D.h
// Author: Antoine Bastide
// Date: 14/01/2025
//

#ifndef PARTICLE_SYSTEM2D_H
#define PARTICLE_SYSTEM2D_H

#include <queue>

#include "Engine2D/Component2D.h"

namespace Engine2D::Rendering {
  class Texture2D;
  class Shader;
}

namespace Engine2D {
  class ParticleSystem2D : public Component2D {
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
      float startLifetime;
      /// The initial position of the particles
      glm::vec2 startPosition;
      /// The initial velocity of the particles
      glm::vec2 startVelocity;
      /// The initial size of the particles
      glm::vec2 startSize;

      /// If the simulation is run in world or local space
      bool simulateInWorldSpace;
      /// How fast the simulation is run, 1.0f runs at normal sped
      float simulationSpeed;
      /// How many particles are simulated per second, a good value for a fluid simulation is maxParticles / simulationSpeed;
      float emissionRate;
      /// The maximum value that will randomly be added/removed from the particles start position
      float maxStartPositionOffset;

      /// The shader of used to render the particles, defaults to the "particle" shader
      std::shared_ptr<Rendering::Shader> shader;
      /// The texture used to render the particles
      std::shared_ptr<Rendering::Texture2D> texture;

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
        /// The size of the particle
        glm::vec2 size;
        /// The current velocity of the particle
        glm::vec2 velocity;
        /// The color of the particle
        glm::vec4 color;
        /// The remaining life span of the particle
        float lifeTime;

        float padding;

        bool Visible() const;
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

      uint instanceVBO;
      uint quadVAO;
      uint quadVBO;
      bool initialized;
      uint aliveCount;

      static const float quadVertices[];

      void OnUpdate() override;
      /// Initializes the particle system
      void initialize();
      /// Renders the particle system
      void render();

      /// Updates the dead particle at the given index and brings it back to life
      void respawnParticle(uint index);
  };
}

#endif //PARTICLE_SYSTEM2D_H
