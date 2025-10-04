//
// EntityInspector.cpp
// Author: Antoine Bastide
// Date: 21.08.2025
//

#include <imgui_internal.h>

#include "Panels/EntityInspector.hpp"
#include "Engine/Reflection/RenderInEditor.hpp"
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
        History::CommandHistory::Create(SceneHierarchy::ToggleActiveCommand(context.get(), isActive));
      }

      ImGui::SameLine();

      Engine::Reflection::InputText("Name", context->name, 1024 * 1024, true);

      ImGui::SameLine();
      bool isStatic = context->isStatic;
      if (ImGui::Checkbox("Static", &isStatic))
        History::CommandHistory::Create(toggleStaticCommand(context, isActive));

      ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetStyle().IndentSpacing * 0.5f);

      int i = 0;
      drawComponent(context->Transform().get(), true, i++);
      for (auto &component: context->allComponents)
        drawComponent(component.get(), false, i++);

      ImGui::PopStyleVar(2);
    }

    ImGui::PopStyleColor();
    ImGui::End();
  }

  void EntityInspector::SetContext(const Engine::Ptr<Engine2D::Entity2D> &entity) {
    context = entity;
  }

  std::unique_ptr<History::EditorCommand> EntityInspector::toggleStaticCommand(
    const Engine::Ptr<Engine2D::Entity2D> &entity, bool isStatic
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
    const std::string name(comp->ClassName());
    const auto id = name + "_" + std::to_string(index);

    ImGui::PushID((comp->Entity()->name + "_" + id).c_str());
    const bool opened = ImGui::TreeNodeEx(
      ("##" + id).c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen |
      ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_AllowItemOverlap,
      ""
    );

    static auto noComps = std::vector<Engine::Ptr<Engine2D::Component2D>>{};
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
      constexpr auto payloadType = "_e_DND_PTR";
      const Engine::Reflection::DragPayload payload{
        nullptr, isTransformComponent ? comp->Transform() : nullptr,
        isTransformComponent ? noComps : comp->Entity()->components
      };
      ImGui::SetDragDropPayload(payloadType, &payload, sizeof(payload));

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
      ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));
      ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 4.0f);

      ImGui::BeginTooltip();
      ImGui::Text("%s (%s)", comp->Entity()->name.c_str(), name.c_str());
      ImGui::EndTooltip();

      ImGui::PopStyleVar(3);
      ImGui::PopStyleColor(2);

      ImGui::EndDragDropSource();
    }

    if (!isTransformComponent) {
      ImGui::SameLine();

      bool isCompActive = comp->active;
      if (ImGui::Checkbox(("##active" + id).c_str(), &isCompActive)) {
        comp->SetActive(isCompActive);
        comp->OnEditorValueChanged();
      }
    }

    ImGui::SameLine();
    ImGui::Text("%s", name.empty() ? "(empty)" : name.c_str());

    if (opened) {
      if (ImGui::BeginTable((id + "Table").c_str(), 2, ImGuiTableFlags_SizingFixedFit)) {
        // Setup columns - fixed width for labels, remaining space for controls
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, ImGui::GetWindowWidth() * 0.35f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        if (const auto customEditor = dynamic_cast<Engine::Reflection::ICustomEditor *>(comp)) {
          if (customEditor->OnRenderInEditor("", false, false))
            comp->OnEditorValueChanged();
        } else if (comp->_e_renderInEditorImpl(false, context->name))
          comp->OnEditorValueChanged();

        ImGui::EndTable();
      }
    }

    ImGui::PopID();
  }
} // Editor
