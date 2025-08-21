//
// DeleteEntityCommand.cpp
// Author: Antoine Bastide
// Date: 18.08.2025
//

#include <sstream>

#include "History/Commands/DeleteEntityCommand.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"

namespace Editor::History {
  DeleteEntityCommand::DeleteEntityCommand(const std::vector<Engine2D::Entity2D *> &entities, Engine2D::Scene *scene)
    : multipleEntitiesDeleted(false), scene(scene), entities(entities) {}

  void DeleteEntityCommand::Execute() {
    multipleEntitiesDeleted = entities.size() > 1;

    std::vector<std::vector<std::unique_ptr<Engine2D::Entity2D>>::iterator> entitiesToErase;
    for (auto it = scene->entities.begin(); it != scene->entities.end(); ++it) {
      if (auto iit = std::ranges::find(entities, it->get()); iit != entities.end()) {
        const bool wasActive = it->get()->IsActive();
        const auto parent = it->get()->Transform()->Parent();
        const auto index = parent ? 0 : std::distance(scene->entities.begin(), it);
        const auto childIndex = parent ? parent->Transform()->ChildIndex(it->get()) : 0;

        it->get()->forceSetParent(nullptr);
        it->get()->SetActive(false);
        if (!multipleEntitiesDeleted)
          multipleEntitiesDeleted = !it->get()->Transform()->Children().empty();

        deletedEntities.emplace_back(index, childIndex, wasActive, std::move(*it), parent);
        entitiesToErase.emplace_back(it);
      }
    }

    std::ranges::sort(entitiesToErase, std::greater<>());
    for (auto it: entitiesToErase)
      scene->entities.erase(it);
    entitiesToErase.clear();
  }

  void DeleteEntityCommand::Undo() {
    for (auto it = deletedEntities.rbegin(); it != deletedEntities.rend(); ++it) {
      it->entity->forceSetParent(it->parent);
      it->entity->SetActive(it->wasActive);
      if (it->parent)
        it->parent->Transform()->MakeNthChild(it->entity.get(), it->childIndex);

      scene->entities.insert(scene->entities.begin() + it->index, std::move(it->entity));
    }
    deletedEntities.clear();
  }

  std::string DeleteEntityCommand::Name() const {
    if (multipleEntitiesDeleted)
      return "Delete Entities";
    return "Delete " + deletedEntities.begin()->entity->name;
  }
}
