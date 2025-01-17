//
// ParticleSystemRenderer2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Common/RenderingHeaders.h"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.h"
#include "Engine2D/ParticleSystem/ParticleSystem2D.h"

namespace Engine2D {
  const float ParticleSystemRenderer2D::vertices[24] = {
    // pos      // tex
    -0.5f, 0.5f, 0.0f, 1.0f,  // Top-left corner
    0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right corner
    -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left corner

    -0.5f, 0.5f, 0.0f, 1.0f, // Top-left corner
    0.5f, 0.5f, 1.0f, 1.0f,  // Top-right corner
    0.5f, -0.5f, 1.0f, 0.0f  // Bottom-right corner
  };

  unsigned int ParticleSystemRenderer2D::VAO{};
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystems;

  void ParticleSystemRenderer2D::initialize() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the particle system buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // Clear
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
  }

  void ParticleSystemRenderer2D::render() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindVertexArray(VAO);

    for (const auto &particleSystem: particleSystems)
      particleSystem->render();

    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void ParticleSystemRenderer2D::addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystems.push_back(particleSystem);
  }

  void ParticleSystemRenderer2D::removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    if (particleSystems.empty())
      return;

    if (const auto it = std::ranges::find(particleSystems, particleSystem); it != particleSystems.end()) {
      particleSystems.erase(it);
    }
  }
} // Engine2D
