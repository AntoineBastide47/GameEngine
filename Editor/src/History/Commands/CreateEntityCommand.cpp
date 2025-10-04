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
    const Engine::Ptr<Engine2D::Scene> &scene, const std::vector<Engine::Ptr<Engine2D::Entity2D>> &parents,
    const std::function<void(Engine::Ptr<Engine2D::Entity2D>)> &createCallback,
    const std::function<void(Engine::Ptr<Engine2D::Entity2D>)> &deleteCallback, const int insertPos
  )
    : insertPos(insertPos), scene(scene), parents(parents), createCallback(createCallback),
      deleteCallback(deleteCallback) {}

  void CreateEntityCommand::Execute() {
    auto create = [&](const Engine::Ptr<Engine2D::Entity2D> &parent, const int index) {
      const auto entity = Engine2D::Entity2D::instantiateAt("Entity", false, {}, 0, {1, 1}, parent, index);
      entity->Transform()->onTransformChange();
      createdEntities.emplace_back(entity.get());

      if (createCallback)
        createCallback(createdEntities.back());
    };

    if (parents.empty())
      create(nullptr, insertPos);
    else {
      createdEntities.resize(parents.size());
      for (const auto &parent: parents)
        create(parent, scene->entities.size() - 1);
    }
  }

  void CreateEntityCommand::Undo() {
    auto del = [&](const Engine::Ptr<Engine2D::Entity2D> &entity) {
      if (entity) {
        if (deleteCallback)
          deleteCallback(entity);
        scene->removeEntity(entity);
      }
    };

    if (parents.empty())
      del(createdEntities.front());
    else
      for (const auto &createdEntity: createdEntities)
        del(createdEntity);
    createdEntities.clear();
  }

  std::string CreateEntityCommand::Name() const {
    if (parents.empty())
      return "Create Entity";
    return "Create Entities";
  }
}
