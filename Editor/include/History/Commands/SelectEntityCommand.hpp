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
        std::vector<Engine2D::Entity2D *> &selectedEntities, const std::vector<Engine2D::Entity2D *> &entities,
        Engine2D::Scene *scene
      );

      void Execute() override;
      void Undo() override;
      std::string Name() const override;
    private:
      struct EntityInfo {
        uint64_t id;
        std::string name;

        explicit EntityInfo(const Engine2D::Entity2D *entity)
          : id(entity->Id()), name(entity->name) {}
      };

      Engine2D::Scene *scene;
      std::vector<EntityInfo> entities;
      std::vector<EntityInfo> oldSelectedEntities;
      std::vector<Engine2D::Entity2D *> &selectedEntities;

      std::vector<Engine2D::Entity2D *> findEntitiesById(const std::vector<EntityInfo> &entityInfos) const;
  };
}

#endif //SELECT_ENTITY_COMMAND_HPP
