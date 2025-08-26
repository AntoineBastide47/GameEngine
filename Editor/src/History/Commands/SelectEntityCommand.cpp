//
// SelectEntityCommand.cpp
// Author: Antoine Bastide
// Date: 18.08.2025
//

#include <sstream>

#include "History/Commands/SelectEntityCommand.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"
#include "Panels/EntityInspector.hpp"

namespace Editor::History {
  SelectEntityCommand::SelectEntityCommand(
    std::vector<Engine2D::Entity2D *> &selectedEntities, const std::vector<Engine2D::Entity2D *> &entities,
    Engine2D::Scene *scene
  )
    : scene(scene), selectedEntities(selectedEntities) {
    this->entities.reserve(entities.size());
    for (const auto entity: entities)
      this->entities.emplace_back(entity);

    oldSelectedEntities.reserve(selectedEntities.size());
    for (const auto entity: selectedEntities)
      oldSelectedEntities.emplace_back(entity);
  }

  void SelectEntityCommand::Execute() {
    selectedEntities = findEntitiesById(entities);
    EntityInspector::SetContext(selectedEntities.size() == 1 ? selectedEntities.front() : nullptr);
  }

  void SelectEntityCommand::Undo() {
    selectedEntities = findEntitiesById(oldSelectedEntities);
    EntityInspector::SetContext(selectedEntities.size() == 1 ? selectedEntities.front() : nullptr);
  }

  std::string SelectEntityCommand::Name() const {
    if (entities.empty())
      return "Clear Selection";

    if (entities.size() == 1) {
      std::stringstream ss;
      ss.write("Select ", 7);
      ss.write(entities.front().name.c_str(), entities.front().name.size());
      return ss.str();
    }

    return "Select Entities";
  }

  bool SelectEntityCommand::AffectsScene() {
    return false;
  }

  std::vector<Engine2D::Entity2D *> SelectEntityCommand::findEntitiesById(
    const std::vector<EntityInfo> &entityInfos
  ) const {
    std::vector<Engine2D::Entity2D *> result;
    result.reserve(entities.size());

    // Find entities by ID in the current scene
    for (const auto &info: entityInfos) {
      for (const auto &entity: scene->entities) {
        if (entity->Id() == info.id && !entity->destroyed) {
          result.push_back(entity.get());
          break;
        }
      }
    }

    return result;
  }
}
