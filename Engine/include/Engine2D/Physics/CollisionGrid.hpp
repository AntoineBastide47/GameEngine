//
// CollisionGrid.h
// Author: Antoine Bastide
// Date: 16/12/2024
//

#ifndef COLLISION_GRID_H
#define COLLISION_GRID_H

#include <vector>
#include <glm/glm.hpp>

#include "Engine/Types/Ptr.hpp"

namespace Engine2D::Physics {
  class Collider2D;

  class CollisionGrid {
    friend class Physics2D;

    glm::vec2 bottomLeft;
    glm::vec2 topRight;
    glm::vec2 cellSize;
    glm::vec<2, size_t> gridSize;
    std::vector<std::vector<std::vector<Engine::Ptr<Collider2D>>>> grid;

    CollisionGrid();
    explicit CollisionGrid(glm::vec<2, size_t> gridSize);
    void setGridSize(glm::vec<2, size_t> gridSize);

    void update(const std::vector<Engine::Ptr<Collider2D>> &colliders);
  };
}

#endif //COLLISION_GRID_H
