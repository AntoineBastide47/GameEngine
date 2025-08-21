//
// SceneHierarchy.hpp
// Author: Antoine Bastide
// Date: 14.08.2025
//

#ifndef SCENE_HIERARCHY_HPP
#define SCENE_HIERARCHY_HPP
#include <unordered_set>

#include "History/CommandHistory.hpp"

namespace Engine2D {
  class Scene;
  class Entity2D;
}

namespace Editor {
  class SceneHierarchy final {
    friend class LevelEditor;
    public:
      /// Render's the scene hierarchy
      static void Render(const ImGuiWindowClass *winClass);
      /// Set's the scene to be rendered
      static void SetContext(Engine2D::Scene *scene);
    private:
      inline static bool recurseSelect;
      inline static Engine2D::Scene *context;
      inline static Engine2D::Entity2D *renamingEntity;
      inline static Engine2D::Entity2D *defaultParent;
      inline static History::CommandHistory commandHistory;
      inline static std::vector<Engine2D::Entity2D *> selectedEntities;
      inline static std::unordered_set<Engine2D::Entity2D *> forceOpenEntities;
      inline static std::unordered_set<Engine2D::Entity2D *> openedTreeNodes;

      /// Draw's an entity node
      static void drawEntityNode(Engine2D::Entity2D *entity);
      /// Draws the popup shown when right-clicking the window
      static void rightClickPopupMenuItems(Engine2D::Entity2D *entity);

      /// Recursively finds all the selected entities
      static std::vector<Engine2D::Entity2D *> calculateRangeSelection(Engine2D::Entity2D *targetEntity);
      /// Finds selected entities
      static void collectVisibleEntities(
        const std::vector<std::unique_ptr<Engine2D::Entity2D>> &entities, std::vector<Engine2D::Entity2D *> &outList, std::
        vector<bool> &isChild
      );
      /// Finds selected entities recursively
      static void collectVisibleEntitiesRecursive(
        Engine2D::Entity2D *entity, std::vector<Engine2D::Entity2D *> &outList, std::vector<bool> &isChild
      );
  };
} // Editor

#endif //SCENE_HIERARCHY_HPP
