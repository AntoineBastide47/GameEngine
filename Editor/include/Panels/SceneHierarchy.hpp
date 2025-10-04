//
// SceneHierarchy.hpp
// Author: Antoine Bastide
// Date: 14.08.2025
//

#ifndef SCENE_HIERARCHY_HPP
#define SCENE_HIERARCHY_HPP
#include <unordered_set>

#include "Engine/Types/Ptr.hpp"
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
      static void SetContext(const Engine::Ptr<Engine2D::Scene> &scene);

      static std::unique_ptr<History::EditorCommand> ToggleActiveCommand(
        const Engine::Ptr<Engine2D::Entity2D> &entity, bool isStatic
      );
    private:
      inline static bool recurseSelect;
      inline static Engine::Ptr<Engine2D::Scene> context;
      inline static Engine::Ptr<Engine2D::Entity2D> renamingEntity;
      inline static Engine::Ptr<Engine2D::Entity2D> defaultParent;
      inline static std::vector<Engine::Ptr<Engine2D::Entity2D>> selectedEntities;
      inline static std::unordered_set<Engine::Ptr<Engine2D::Entity2D>> forceOpenEntities;
      inline static std::unordered_set<Engine::Ptr<Engine2D::Entity2D>> openedTreeNodes;

      /// Draw's an entity node
      static void drawEntityNode(const Engine::Ptr<Engine2D::Entity2D> &entity);
      /// Draws the popup shown when right-clicking the window
      static void rightClickPopupMenuItems(const Engine::Ptr<Engine2D::Entity2D> &entity);

      /// Recursively finds all the selected entities
      static std::vector<Engine::Ptr<Engine2D::Entity2D>> calculateRangeSelection(
        const Engine::Ptr<Engine2D::Entity2D> &targetEntity
      );
      /// Finds selected entities
      static void collectVisibleEntities(
        const std::vector<std::unique_ptr<Engine2D::Entity2D>> &entities,
        std::vector<Engine::Ptr<Engine2D::Entity2D>> &outList,
        std::vector<bool> &isChild
      );
      /// Finds selected entities recursively
      static void collectVisibleEntitiesRecursive(
        const Engine::Ptr<Engine2D::Entity2D> &entity, std::vector<Engine::Ptr<Engine2D::Entity2D>> &outList,
        std::vector<bool> &isChild
      );
  };
} // Editor

#endif //SCENE_HIERARCHY_HPP
