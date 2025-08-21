//
// CreateEntityCommand.cpp
// Author: Antoine Bastide
// Date: 17.08.2025
//

#include "History/Commands/CreateEntityCommand.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"

namespace Editor::History {
  CreateEntityCommand::CreateEntityCommand(
    Engine2D::Scene *scene, const std::vector<Engine2D::Entity2D *> &parents,
    const std::function<void(Engine2D::Entity2D *)> &createCallback,
    const std::function<void(Engine2D::Entity2D *)> &deleteCallback, const int insertPos
  )
    : insertPos(insertPos), scene(scene), parents(parents), createCallback(createCallback),
      deleteCallback(deleteCallback) {}

  void CreateEntityCommand::Execute() {
    auto create = [&](Engine2D::Entity2D *parent, const int index) {
      auto entity = std::unique_ptr<Engine2D::Entity2D>(new Engine2D::Entity2D("Entity", false, {}, 0, {}, parent));
      entity->id = Engine2D::Entity2D::nextId++;

      createdEntities.emplace_back(entity.get());
      scene->entities.insert(scene->entities.begin() + index, std::move(entity));

      if (createCallback)
        createCallback(createdEntities.back());
    };

    if (parents.empty())
      create(nullptr, insertPos);
    else {
      createdEntities.resize(parents.size());
      for (const auto parent: parents)
        create(parent, scene->entities.size() - 1);
    }
  }

  void CreateEntityCommand::Undo() {
    auto del = [&](Engine2D::Entity2D *entity) {
      if (entity) {
        if (deleteCallback)
          deleteCallback(entity);
        scene->removeEntity(entity);
      }
    };

    if (parents.empty())
      del(createdEntities.front());
    else
      for (const auto createdEntity: createdEntities)
        del(createdEntity);
    createdEntities.clear();
  }

  std::string CreateEntityCommand::Name() const {
    if (parents.empty())
      return "Create Entity";
    return "Create Entities";
  }
}
