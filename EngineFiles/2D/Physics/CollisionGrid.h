//
// CollisionGrid.h
// Author: Antoine Bastide
// Date: 16/12/2024
//

#ifndef COLLISION_GRID_H
#define COLLISION_GRID_H

#include "2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Rigidbody2D;
}

namespace Engine2D::Physics {
  class CollisionGrid {
    friend class Physics2D;

    Vector2f bottomLeft;
    Vector2f topRight;
    Vector2f cellSize;
    Vector2<size_t> gridSize;
    std::vector<std::vector<std::vector<Rigidbody2D *> > > grid;

    explicit CollisionGrid(Vector2<size_t> gridSize);
    void setGridSize(Vector2<size_t> gridSize);

    void update(const std::vector<Rigidbody2D *> &rigidbodies);
    std::vector<std::tuple<Rigidbody2D *, Rigidbody2D *> > collisionPairs() const;
  };
}

template<typename T1, typename T2> struct std::hash<std::pair<T1, T2> > {
  size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    // Combine the two hash values (use a prime multiplier to avoid collisions)
    return h1 ^ h2 << 1;
  }
};

#endif //COLLISION_GRID_H
