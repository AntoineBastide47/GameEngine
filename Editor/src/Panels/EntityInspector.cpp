//
// EntityInspector.cpp
// Author: Antoine Bastide
// Date: 21.08.2025
//

#include <imgui_internal.h>

#include "Panels/EntityInspector.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"
#include "History/CommandHistory.hpp"
#include "Panels/SceneHierarchy.hpp"

namespace Editor {
  void EntityInspector::Render(const ImGuiWindowClass *winClass) {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::SetNextWindowClass(winClass);
    ImGui::Begin("Inspector", nullptr, ImGuiDockNodeFlags_NoTabBar);

    if (context) {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

      bool isActive = context->active;
      if (ImGui::Checkbox("##active", &isActive)) {
        History::CommandHistory::Create(SceneHierarchy::ToggleActiveCommand(context, isActive));
      }

      ImGui::SameLine();

      char buffer[256] = {};
      strncpy(buffer, context->name.c_str(), sizeof(buffer) - 1);
      buffer[sizeof(buffer) - 1] = '\0';
      if (ImGui::InputText("##name", buffer, sizeof(buffer))) {
        context->name = buffer;
      }

      ImGui::SameLine();
      bool isStatic = context->isStatic;
      if (ImGui::Checkbox("Static", &isStatic)) {
        History::CommandHistory::Create(toggleStaticCommand(context, isActive));
      }

      ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetStyle().IndentSpacing * 0.5f);

      int i = 0;
      drawComponent(context->Transform(), true, i++);
      for (auto &component: context->allComponents)
        drawComponent(component.get(), false, i++);

      ImGui::PopStyleVar(2);
    }

    ImGui::PopStyleColor();
    ImGui::End();
  }

  void EntityInspector::SetContext(Engine2D::Entity2D *entity) {
    context = entity;
  }

  std::unique_ptr<History::EditorCommand> EntityInspector::toggleStaticCommand(
    Engine2D::Entity2D *entity, bool isStatic
  ) {
    return History::MakeLambdaCommand(
      "Toggle " + entity->name + (isStatic ? " active" : " inactive"), [entity, isStatic]() {
        entity->isStatic = isStatic;
      }, [entity, isStatic]() {
        entity->isStatic = !isStatic;
      }
    );
  }

  void EntityInspector::drawComponent(Engine2D::Component2D *comp, const bool isTransformComponent, const int index) {
    const auto id = std::string(comp->ClassName()) + "_" + std::to_string(index);
    const std::string_view name(id.data(), id.find('_'));

    ImGui::PushID((comp->Entity()->name + "_" + id).c_str());
    const bool opened = ImGui::TreeNodeEx(
      ("##" + id).c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen |
      ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_AllowItemOverlap,
      ""
    );

    if (!isTransformComponent) {
      ImGui::SameLine();

      bool isCompActive = comp->active;
      if (ImGui::Checkbox(("##active" + id).c_str(), &isCompActive)) {
        comp->SetActive(isCompActive);
        comp->OnEditorValueChanged();
      }
    }

    ImGui::SameLine();

    ImGui::Text("%.*s", static_cast<int>(name.size()), name.data() ? name.data() : "(empty)");

    if (opened) {
      if (ImGui::BeginTable((id + "Table").c_str(), 2, ImGuiTableFlags_SizingFixedFit)) {
        // Setup columns - fixed width for labels, remaining space for controls
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, ImGui::GetWindowWidth() * 0.35f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        if (comp->_e_renderInEditor(false))
          comp->OnEditorValueChanged();

        ImGui::EndTable();
      }
    }

    ImGui::PopID();
  }
} // Editor
