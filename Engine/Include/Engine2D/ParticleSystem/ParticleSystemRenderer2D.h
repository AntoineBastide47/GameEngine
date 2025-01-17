//
// ParticleSystemRenderer2D.h
// Author: Antoine Bastide
// Date: 16/01/2025
//

#ifndef PARTICLE_SYSTEM_RENDERER2D_H
#define PARTICLE_SYSTEM_RENDERER2D_H

#include <vector>

namespace Engine2D {
  class ParticleSystem2D;
}
namespace Engine2D {
  class ParticleSystemRenderer2D {
    friend class Entity2D;
    friend class Game2D;

    /// The vertices that will be sent to the VAO
    static const float vertices[24];
    /// The vertex array buffer used for rendering
    static unsigned int VAO;
    static std::vector<std::shared_ptr<ParticleSystem2D>> particleSystems;

    static void initialize();
    static void render();

    static void addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem);
    static void removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem);
  };
} // Engine2D

#endif //PARTICLE_SYSTEM_RENDERER2D_H
