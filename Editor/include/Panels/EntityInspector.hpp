//
// EntityInspector.hpp
// Author: Antoine Bastide
// Date: 21.08.2025
//

#ifndef ENTITY_INSPECTOR_HPP
#define ENTITY_INSPECTOR_HPP

#include <imgui.h>

#include "Engine2D/Entity2D.hpp"
#include "History/EditorCommand.hpp"
#include "Engine/Reflection/RenderInEditor.hpp"

namespace Editor {
  class EntityInspector final {
    public:
      /// Render's the scene hierarchy
      static void Render(const ImGuiWindowClass *winClass);
      /// Set's the scene to be rendered
      static void SetContext(Engine2D::Entity2D *entity);
    private:
      inline static Engine2D::Entity2D *context;

      static std::unique_ptr<History::EditorCommand> toggleStaticCommand(Engine2D::Entity2D *entity, bool isStatic);

      static void drawComponent(Engine2D::Component2D *comp, bool isTransformComponent, int index);
  };
} // Editor

#endif //ENTITY_INSPECTOR_HPP
