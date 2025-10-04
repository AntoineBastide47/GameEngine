//
// CollisionGrid.cpp
// Author: Antoine Bastide
// Date: 16/12/2024
//

#include "Engine2D/Physics/CollisionGrid.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Physics/Collider2D.hpp"

namespace Engine2D::Physics {
  CollisionGrid::CollisionGrid()
    : bottomLeft(), topRight(), cellSize(), gridSize() {}

  CollisionGrid::CollisionGrid(const glm::vec<2, size_t> gridSize) {
    this->bottomLeft = glm::vec2(Game2D::ViewportWidth() * -0.5f, Game2D::ViewportHeight() * -0.5f);
    this->topRight = -bottomLeft;
    this->gridSize = gridSize;
    this->cellSize = glm::vec2(Game2D::ViewportWidth() / gridSize.x, Game2D::ViewportHeight() / gridSize.y);
    grid.resize(gridSize.x, std::vector<std::vector<Engine::Ptr<Collider2D>>>(gridSize.y));
  }

  void CollisionGrid::setGridSize(const glm::vec<2, size_t> gridSize) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    this->gridSize = gridSize;
    for (auto &col: grid)
      for (auto &cell: col)
        cell.clear();
    grid.resize(gridSize.x, std::vector<std::vector<Engine::Ptr<Collider2D>>>(gridSize.y));
  }

  void CollisionGrid::update(const std::vector<Engine::Ptr<Collider2D>> &colliders) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    // Clear the cells
    for (auto &col: grid)
      for (auto &cell: col)
        cell.clear();

    for (const auto &collider: colliders) {
      const auto [min, max] = collider->getAABB();

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
          grid[x][y].push_back(collider);
    }
  }
}
