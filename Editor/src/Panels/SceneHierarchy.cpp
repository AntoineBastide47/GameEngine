//
// SceneHierarchy.cpp
// Author: Antoine Bastide
// Date: 14.08.2025
//

#include <filesystem>
#include <imgui.h>

#include "Panels/SceneHierarchy.hpp"
#include "imgui_internal.h"
#include "ProjectLoader.hpp"
#include "../../../Engine/include/Engine2D/Game2D.hpp"
#include "Engine/Reflection/Deserializer.hpp"
#include "Engine/Reflection/RenderInEditor.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"
#include "History/Commands/CreateEntityCommand.hpp"
#include "History/Commands/DeleteEntityCommand.hpp"
#include "History/Commands/SelectEntityCommand.hpp"
#include "Panels/EntityInspector.hpp"
#include "Panels/SceneViewport.hpp"

namespace Editor {
  std::unique_ptr<History::EditorCommand> SceneHierarchy::ToggleActiveCommand(
    Engine2D::Entity2D *entity, bool isStatic
  ) {
    return History::MakeLambdaCommand(
      "Toggle " + entity->name + (isStatic ? " active" : " inactive"), [entity, isStatic]() {
        entity->SetActive(isStatic);
      }, [entity, isStatic]() {
        entity->SetActive(!isStatic);
      }
    );
  }

  static std::unique_ptr<History::EditorCommand> RenameEntityCommand(Engine2D::Entity2D *entity, std::string newName) {
    return History::MakeLambdaCommand(
      "Rename " + entity->name + " to " + newName, [entity, newName]() {
        entity->name = newName;
      }, [entity, name=entity->name]() {
        entity->name = name;
      }
    );
  }

  static std::unique_ptr<History::EditorCommand> DefaultParentCommand(
    Engine2D::Entity2D **defaultParent, Engine2D::Entity2D *entity
  ) {
    return History::MakeLambdaCommand(
      "Set Default Parent", [defaultParent,entity]() {
        *defaultParent = entity;
      }, [defaultParent, old = *defaultParent]() {
        *defaultParent = old;
      }
    );
  }

  void SceneHierarchy::Render(const ImGuiWindowClass *winClass) {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::SetNextWindowClass(winClass);
    ImGui::Begin("Hierarchy", nullptr, ImGuiDockNodeFlags_NoTabBar);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetStyle().IndentSpacing * 0.5f);

    if (context) {
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

      constexpr ImGuiTreeNodeFlags flags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
          ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
          ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
      const bool contextChanged = History::CommandHistory::HasAffectedScene();
      const bool opened = ImGui::TreeNodeEx(context, flags, "%s (Scene)", context->name.c_str());

      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
      if (ImGui::Button("...##scene_menu")) {
        ImGui::OpenPopup("SceneContextMenu");
      }
      ImGui::PopStyleVar();

      const std::string path = ProjectLoader::GetProjectPath() + '/' + context->name + ".json";

      if (ImGui::BeginPopup("SceneContextMenu")) {
        if (ImGui::MenuItem("Save Scene"))
          Engine2D::SceneManager::SaveScene(context->name, path, Engine::Reflection::JSON);

        ImGui::Separator();

        if (ImGui::MenuItem("Discard Changes", "", false, std::filesystem::exists(path) && contextChanged)) {
          auto &scene = Engine2D::SceneManager::getSceneRef(context->name);
          scene = Engine::Reflection::Deserializer::FromJsonFromFile<std::unique_ptr<Engine2D::Scene>>(path);

          SetContext(scene.get());
          Engine2D::SceneManager::activeScene = scene.get();
          SceneViewport::resizeSceneFrameBuffer(
            SceneViewport::frameBufferData.width, SceneViewport::frameBufferData.height
          );
        }

        ImGui::EndPopup();
      }

      if (opened) {
        for (const auto &entity: context->entities)
          if (!entity->destroyed && !entity->Transform()->Parent())
            drawEntityNode(entity.get());

        ImGui::TreePop();
      }

      // Add empty space at the bottom for dropping
      if (const ImVec2 remainingSpace = ImGui::GetContentRegionAvail(); remainingSpace.y > 20) {
        if (ImGui::InvisibleButton("##empty_space", ImVec2(-1, remainingSpace.y)) && !selectedEntities.empty())
          History::CommandHistory::Create(
            std::make_unique<History::SelectEntityCommand>(
              selectedEntities, std::vector<Engine2D::Entity2D *>{}, context
            )
          );

        if (ImGui::BeginPopupContextItem("##empty_space_popup")) {
          rightClickPopupMenuItems(nullptr);
          ImGui::EndPopup();
        }

        // Handle dropping on empty space (unparent)
        if (ImGui::BeginDragDropTarget()) {
          if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            if (const Engine2D::Entity2D *draggedEntity = *static_cast<Engine2D::Entity2D **>(payload->Data)) {
              draggedEntity->Transform()->SetParent(nullptr);
            }
          }
          ImGui::EndDragDropTarget();
        }
      }
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::End();

    History::CommandHistory::executePendingCommands();
  }

  void SceneHierarchy::SetContext(Engine2D::Scene *scene) {
    if (context != scene) {
      History::CommandHistory::Clear();
      EntityInspector::SetContext(nullptr);
      selectedEntities.clear();
      forceOpenEntities.clear();
      openedTreeNodes.clear();
      renamingEntity = nullptr;
      recurseSelect = false;
    }
    context = scene;
  }

  static char buffer[256] = {};

  void SceneHierarchy::drawEntityNode(Engine2D::Entity2D *entity) {
    const bool selected = std::ranges::find(selectedEntities, entity) != selectedEntities.end();
    const ImGuiTreeNodeFlags flags =
        (selected ? ImGuiTreeNodeFlags_Selected : 0) |
        (entity->Transform()->ChildCount() > 0
           ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
           : ImGuiTreeNodeFlags_Leaf) | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_AllowItemOverlap;

    if (forceOpenEntities.contains(entity)) {
      ImGui::SetNextItemOpen(true);
      forceOpenEntities.erase(entity);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    if (!selected && defaultParent == entity)
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    else if (!selected && defaultParent != entity)
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    else
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.26f, 0.59f, 0.98f, 0.31f));

    bool opened;
    if (renamingEntity == entity) {
      if (!entity->Transform()->Children().empty())
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().IndentSpacing * 1.2f);
      opened = ImGui::TreeNodeEx(reinterpret_cast<void *>(entity->Id()), flags, "");

      strncpy(buffer, entity->name.c_str(), sizeof(buffer) - 1);
      buffer[sizeof(buffer) - 1] = '\0';

      ImGui::SameLine();
      ImGui::SetKeyboardFocusHere();
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
      ImGui::SetCursorPosX(
        ImGui::GetCursorPosX() - ImGui::GetStyle().IndentSpacing * (
          0.8f + entity->Transform()->Children().empty() * 1.2f)
      );

      if (Engine::Reflection::InputText("rename", entity->name, 1024 * 1024, true))
        renamingEntity = nullptr;
      ImGui::PopStyleVar();
    } else {
      if (!entity->Transform()->Children().empty())
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().IndentSpacing * 1.2f);
      opened = ImGui::TreeNodeEx(reinterpret_cast<void *>(entity->Id()), flags, "%s", entity->name.c_str());

      if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter) &&
          selectedEntities.size() == 1 && std::ranges::find(selectedEntities, entity) != selectedEntities.end()) {
        renamingEntity = entity;
      }

      // Handle arrow key navigation
      if (ImGui::IsItemFocused() &&
          (ImGui::IsKeyReleased(ImGuiKey_UpArrow) || ImGui::IsKeyReleased(ImGuiKey_DownArrow))) {
        History::CommandHistory::Create(
          std::make_unique<History::SelectEntityCommand>(selectedEntities, std::vector{entity}, context)
        );
      }

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
        std::vector<Engine2D::Entity2D *> newSelection;

        const auto &io = ImGui::GetIO();
        const bool shiftPressed = io.KeyShift;
        const bool ctrlPressed = io.KeyCtrl || io.KeySuper;

        if (shiftPressed && !selectedEntities.empty()) {
          newSelection = calculateRangeSelection(entity);
        } else if (ctrlPressed) {
          newSelection = selectedEntities;
          if (const auto it = std::ranges::find(newSelection, entity); it != newSelection.end())
            newSelection.erase(it);
          else
            newSelection.push_back(entity);
        } else
          newSelection = {entity};

        History::CommandHistory::Create(
          std::make_unique<History::SelectEntityCommand>(selectedEntities, newSelection, context)
        );
      }

      // Drag and Drop Source
      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("ENTITY_NODE", &entity, sizeof(Engine2D::Entity2D *));
        ImGui::Text("%s (Entity2D)", entity->name.c_str());
        ImGui::EndDragDropSource();
      }

      // Drag and Drop Target
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
          if (const Engine2D::Entity2D *draggedEntity = *static_cast<Engine2D::Entity2D **>(payload->Data);
            draggedEntity && draggedEntity != entity && !draggedEntity->Transform()->IsChildOf(entity)) {
            draggedEntity->Transform()->SetParent(entity);
          }
        }
        ImGui::EndDragDropTarget();
      }
    }

    if (opened)
      openedTreeNodes.insert(entity);
    else
      openedTreeNodes.erase(entity);

    if (ImGui::BeginPopupContextItem()) {
      rightClickPopupMenuItems(entity);
      ImGui::EndPopup();
    }

    ImGui::SameLine();

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 35);
    bool isActive = entity->active;
    ImGui::SetItemAllowOverlap();
    if (ImGui::Checkbox(("##" + std::to_string(entity->Id())).c_str(), &isActive))
      History::CommandHistory::Create(ToggleActiveCommand(entity, isActive));
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    if (opened) {
      for (const auto &child: *entity->Transform())
        if (child && !child->destroyed)
          drawEntityNode(child);
      ImGui::TreePop();
    }
  }

  void SceneHierarchy::rightClickPopupMenuItems(Engine2D::Entity2D *entity) {
    if (entity && std::ranges::find(selectedEntities, entity) == selectedEntities.end()) {
      History::CommandHistory::Create(
        std::make_unique<History::SelectEntityCommand>(selectedEntities, std::vector{entity}, context)
      );
      recurseSelect = false;
    }

    const bool clickedAndSelected = entity && selectedEntities.size() == 1;
    if (ImGui::MenuItem("Rename", "", false, clickedAndSelected)) {
      renamingEntity = entity;
    }

    if (ImGui::MenuItem("Delete", "", false, entity)) {
      if (!selectedEntities.empty()) {
        History::CommandHistory::Create(std::make_unique<History::DeleteEntityCommand>(selectedEntities, context));
        selectedEntities.clear();
      } else
        History::CommandHistory::Create(std::make_unique<History::DeleteEntityCommand>(std::vector{entity}, context));
    }

    ImGui::Separator();

    if (const bool set = !((!entity && defaultParent) || (clickedAndSelected && defaultParent == entity));
      ImGui::MenuItem(
        set ? "Set as Default Parent" : "Clear Default Parent", "", false,
        (!entity && defaultParent) || selectedEntities.size() == 1
      )) {
      History::CommandHistory::Create(DefaultParentCommand(&defaultParent, set ? entity : nullptr));
    }

    ImGui::Separator();

    if (ImGui::MenuItem(entity ? "Create Empty Child" : "Create Empty")) {
      if (!selectedEntities.empty()) {
        History::CommandHistory::Create(
          std::make_unique<History::CreateEntityCommand>(context, selectedEntities, nullptr, nullptr)
        );
        forceOpenEntities.insert(selectedEntities.begin(), selectedEntities.end());
      } else {
        History::CommandHistory::Create(
          std::make_unique<History::CreateEntityCommand>(
            context, std::vector{entity ? entity : defaultParent}, [](Engine2D::Entity2D *newEntity) {
              renamingEntity = newEntity;
            }, nullptr
          )
        );
        forceOpenEntities.insert(entity ? entity : defaultParent);
      }
    }

    if (ImGui::MenuItem("Create Empty Parent", "", false, entity || (selectedEntities.size() > 1 && !recurseSelect))) {
      auto createCallback = [entity](Engine2D::Entity2D *parent) {
        if (selectedEntities.empty()) {
          entity->forceSetParent(parent);
          forceOpenEntities.insert(parent);
        } else {
          for (const auto e: selectedEntities)
            e->forceSetParent(parent);
          forceOpenEntities.insert(parent);
        }
      };
      auto deleteCallback = [entity](Engine2D::Entity2D *) {
        if (selectedEntities.empty() && entity)
          entity->forceSetParent(nullptr);
        else
          for (const auto e: selectedEntities)
            e->forceSetParent(nullptr);
      };

      int insertPos = INT_MAX;
      if (!selectedEntities.empty()) {
        int index = 0;
        std::unordered_map<Engine2D::Entity2D *, int> entityToIndexMap;
        for (const auto &e: context->entities)
          entityToIndexMap.emplace(e.get(), index++);

        for (const auto e: selectedEntities)
          if (entityToIndexMap.at(e) < insertPos)
            insertPos = entityToIndexMap.at(e);
      } else
        insertPos = -1;

      History::CommandHistory::Create(
        std::make_unique<History::CreateEntityCommand>(
          context, std::vector<Engine2D::Entity2D *>{}, createCallback, deleteCallback, insertPos
        )
      );
    }
  }

  std::vector<Engine2D::Entity2D *> SceneHierarchy::calculateRangeSelection(Engine2D::Entity2D *targetEntity) {
    if (selectedEntities.empty()) {
      return {targetEntity};
    }

    // Get the last selected entity as the range start
    const Engine2D::Entity2D *rangeStart = *selectedEntities.rbegin();

    // Build a flat list of all visible entities in display order
    std::vector<Engine2D::Entity2D *> allVisibleEntities;
    std::vector<bool> isChild;
    collectVisibleEntities(context->entities, allVisibleEntities, isChild);

    recurseSelect = false;
    for (const auto is: isChild)
      if (is) {
        recurseSelect = true;
        break;
      }

    // Find indices of range start and end
    auto startIt = std::ranges::find(allVisibleEntities, rangeStart);
    auto endIt = std::ranges::find(allVisibleEntities, targetEntity);

    if (startIt == allVisibleEntities.end() || endIt == allVisibleEntities.end()) {
      return {targetEntity};
    }

    // Ensure start comes before end
    if (startIt > endIt) {
      std::swap(startIt, endIt);
    }

    // Return all entities in the range (inclusive)
    return std::vector(startIt, endIt + 1);
  }

  void SceneHierarchy::collectVisibleEntities(
    const std::vector<std::unique_ptr<Engine2D::Entity2D>> &entities, std::vector<Engine2D::Entity2D *> &outList,
    std::vector<bool> &isChild
  ) {
    for (const auto &entity: entities) {
      if (!entity->destroyed && !entity->Transform()->Parent()) {
        outList.push_back(entity.get());
        isChild.push_back(false);
        collectVisibleEntitiesRecursive(entity.get(), outList, isChild);
      }
    }
  }

  void SceneHierarchy::collectVisibleEntitiesRecursive(
    Engine2D::Entity2D *entity, std::vector<Engine2D::Entity2D *> &outList, std::vector<bool> &isChild
  ) {
    if (openedTreeNodes.contains(entity)) {
      for (const auto &child: *entity->Transform()) {
        outList.push_back(child);
        isChild.push_back(true);
        collectVisibleEntitiesRecursive(child, outList, isChild);
      }
    }
  }
} // Editor
