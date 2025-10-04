//
// SelectEntityCommand.hpp
// Author: Antoine Bastide
// Date: 18.08.2025
//

#ifndef SELECT_ENTITY_COMMAND_HPP
#define SELECT_ENTITY_COMMAND_HPP

#include "Engine2D/Entity2D.hpp"
#include "History/EditorCommand.hpp"

namespace Editor::History {
  class SelectEntityCommand final : public EditorCommand {
    public:
      SelectEntityCommand(
        std::vector<Engine::Ptr<Engine2D::Entity2D>> &selectedEntities,
        const std::vector<Engine::Ptr<Engine2D::Entity2D>> &entities, const Engine::Ptr<Engine2D::Scene> &scene
      );

      void Execute() override;
      void Undo() override;
      std::string Name() const override;

      bool AffectsScene() override;
    private:
      struct EntityInfo {
        uint64_t id;
        std::string name;

        explicit EntityInfo(const Engine::Ptr<Engine2D::Entity2D> &entity)
          : id(entity->Id()), name(entity->name) {}
      };

      Engine::Ptr<Engine2D::Scene> scene;
      std::vector<EntityInfo> entities;
      std::vector<EntityInfo> oldSelectedEntities;
      std::vector<Engine::Ptr<Engine2D::Entity2D>> &selectedEntities;

      std::vector<Engine::Ptr<Engine2D::Entity2D>> findEntitiesById(const std::vector<EntityInfo> &entityInfos) const;
  };
}

#endif //SELECT_ENTITY_COMMAND_HPP
