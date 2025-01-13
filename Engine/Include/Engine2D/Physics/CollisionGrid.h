//
// CollisionGrid.h
// Author: Antoine Bastide
// Date: 16/12/2024
//

#ifndef COLLISION_GRID_H
#define COLLISION_GRID_H

#include "Engine2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Collider2D;
}

namespace Engine2D::Physics {
  class CollisionGrid {
    friend class Physics2D;

    Vector2f bottomLeft;
    Vector2f topRight;
    Vector2f cellSize;
    Vector2<size_t> gridSize;
    std::vector<std::vector<std::vector<std::shared_ptr<Collider2D>>>> grid;

    explicit CollisionGrid(Vector2<size_t> gridSize);
    void setGridSize(Vector2<size_t> gridSize);

    void update(const std::vector<std::shared_ptr<Collider2D>> &colliders);
  };
}

#endif //COLLISION_GRID_H
