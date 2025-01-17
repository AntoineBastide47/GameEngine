//
// ParticleSystem2D.h
// Author: Antoine Bastide
// Date: 14/01/2025
//

#ifndef PARTICLE_SYSTEM2D_H
#define PARTICLE_SYSTEM2D_H

#include <queue>

#include "Common/Property.h"
#include "Engine2D/Component2D.h"
#include "Engine2D/Types/Vector2.h"

namespace Engine2D::Rendering {
  class Texture2D;
  class Shader;
}

namespace Engine2D {
  /// Represents a single particle and it's state
  struct Particle {
    /// The position of the particle
    Vector2f position;
    /// The size of the particle
    Vector2f size;
    /// The current velocity of the particle
    Vector2f velocity;
    /// The color of the particle
    glm::vec4 color;
    /// The remaining life span of the particle
    float duration;

    Particle(): color(1.0f), duration(0.0f) {}

    bool Visible() const;
  };

  class ParticleSystem2D : public Component2D {
    friend class Game2D;
    friend class Entity2D;
    friend class ParticleSystemRenderer2D;
    public:
      /// How long the particle system will be simulated
      Engine::Property<float> duration;
      /// If the particle system restarts after it finished simulating itself
      bool loop;

      /// How long to wait before running the first simulation
      float startDelay;
      /// How long particles will be visible on screen
      float startLifetime;
      /// The initial position of the particles
      Vector2f startPosition;
      /// The initial velocity of the particles
      Vector2f startVelocity;
      /// The initial size of the particles
      Vector2f startSize;

      /// If the simulation is run in world or local space
      bool simulateInWorldSpace;
      /// How fast the simulation is run, 1.0f runs at normal sped
      float simulationSpeed;
      /// The maximum amount of particles that can be simulated
      Engine::Property<unsigned int> maxParticles;
      /// How many particles are simulated per second, a good value for a fluid simulation is maxParticles / simulationSpeed;
      float emissionRate;
      /// The maximum value that will randomly be added/removed from the particles start position
      float maxStartPositionOffset;

      /// The shader of used to render the particles, defaults to the "particle" shader
      std::shared_ptr<Rendering::Shader> shader;
      /// The texture used to render the particles
      std::shared_ptr<Rendering::Texture2D> texture;

      ParticleSystem2D();
    private:
      /// The list of all the simulated particles
      std::vector<Particle> particles;
      /// Emission accumulator timer
      float emissionAcc;
      /// Duration accumulator timer
      float durationAcc;

      /// Whether we need to update the particles or not
      bool skipUpdate;
      /// Whether the simulation is done or not
      bool simulationFinished;
      /// Whether we can forcefully skip the update and render steps
      bool forceSkipUpdateAndRendering;

      /// The index of the last dead particle in the list
      unsigned int lastUsedParticle;

      void OnUpdate() override;
      /// Renders the particle system
      void render();

      /// Updates the dead particle at the given index and brings it back to life
      void respawnParticle(unsigned int index);
  };
}

#endif //PARTICLE_SYSTEM2D_H
