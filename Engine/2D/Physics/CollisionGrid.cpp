//
// CollisionGrid.cpp
// Author: Antoine Bastide
// Date: 16/12/2024
//

#include "2D/Physics/CollisionGrid.h"
#include "2D/Game2D.h"
#include "2D/Components/Transform2D.h"
#include "2D/Physics/Collisions.h"
#include "2D/Physics/Rigidbody2D.h"

namespace Engine2D::Physics {
  CollisionGrid::CollisionGrid(const Vector2<size_t> gridSize) {
    this->bottomLeft = Vector2f{Game2D::ViewportWidth() * -0.5f, Game2D::ViewportHeight() * -0.5f};
    this->topRight = -bottomLeft;
    this->gridSize = gridSize;
    this->cellSize = Vector2f{Game2D::ViewportWidth() / gridSize.x, Game2D::ViewportHeight() / gridSize.y};
    grid.resize(gridSize.x, std::vector<std::vector<std::shared_ptr<Rigidbody2D> > >(gridSize.y));
  }

  void CollisionGrid::setGridSize(const Vector2<size_t> gridSize) {
    this->gridSize = gridSize;
    for (auto &col: grid)
      for (auto &cell: col)
        cell.clear();
    grid.resize(gridSize.x, std::vector<std::vector<std::shared_ptr<Rigidbody2D> > >(gridSize.y));
  }

  void CollisionGrid::update(const std::vector<std::shared_ptr<Rigidbody2D> > &rigidbodies) {
    // Clear the cells
    for (auto &col: grid)
      for (auto &cell: col)
        cell.clear();

    for (const auto rigidbody: rigidbodies) {
      // Early out for out of view rigidbodies
      if (!rigidbody->Transform()->Visible())
        continue;

      const auto [min, max] = rigidbody->getAABB();

      const int xBodyMin = std::clamp(
        static_cast<int>(std::floor((min.x - bottomLeft.x) / cellSize.x)), 0, static_cast<int>(gridSize.x) - 1
      );
      const int yBodyMin = std::clamp(
        static_cast<int>(std::floor((min.y - bottomLeft.y) / cellSize.y)), 0, static_cast<int>(gridSize.y) - 1
      );
      const int xBodyMax = std::clamp(
        static_cast<int>(std::floor((max.x - bottomLeft.x) / cellSize.x)), 0, static_cast<int>(gridSize.x) - 1
      );
      const int yBodyMax = std::clamp(
        static_cast<int>(std::floor((max.y - bottomLeft.y) / cellSize.y)), 0, static_cast<int>(gridSize.y) - 1
      );

      for (int x = xBodyMin; x <= xBodyMax; ++x)
        for (int y = yBodyMin; y <= yBodyMax; ++y)
          grid[x][y].push_back(rigidbody);
    }
  }

  std::vector<std::tuple<std::shared_ptr<Rigidbody2D>, std::shared_ptr<Rigidbody2D> > >
  CollisionGrid::collisionPairs() const {
    std::vector<std::tuple<std::shared_ptr<Rigidbody2D>, std::shared_ptr<Rigidbody2D> > > collisions;

    for (auto &gridCol: grid) {
      for (auto &gridCell: gridCol) {
        for (std::size_t i = 0; i < gridCell.size(); ++i) {
          auto rb1 = gridCell[i];
          const Rigidbody2D::AABB rb1AABB = rb1->getAABB();
          for (std::size_t j = i + 1; j < gridCell.size(); ++j) {
            auto rb2 = gridCell[j];
            // Do nothing if both colliders are static
            if (rb1->isStatic && rb2->isStatic)
              continue;

            // Cheap collision check for early out
            if (!Collisions::collideAABB(rb1AABB, rb2->getAABB()))
              continue;

            std::pair<std::shared_ptr<Rigidbody2D>, std::shared_ptr<Rigidbody2D> > bodyPair = rb1 < rb2
              ? std::make_pair(rb1, rb2)
              : std::make_pair(rb2, rb1);

            bool isDuplicate = false;
            for (auto it = collisions.begin(); it != collisions.end(); ++it) {
              if (auto [fst, snd] = std::make_pair(std::get<0>(*it), std::get<1>(*it));
                fst == bodyPair.first && snd == bodyPair.second) {
                isDuplicate = true;
                break;
              }
            }
            if (!isDuplicate)
              collisions.push_back({bodyPair.first, bodyPair.second});
          }
        }
      }
    }

    return std::vector(collisions.begin(), collisions.end());
  }
}
