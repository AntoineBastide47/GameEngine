//
// DeleteEntityCommand.hpp
// Author: Antoine Bastide
// Date: 18.08.2025
//

#ifndef DELETE_ENTITY_COMMAND_HPP
#define DELETE_ENTITY_COMMAND_HPP

#include <map>

#include "Engine2D/Entity2D.hpp"
#include "History/EditorCommand.hpp"

namespace Editor::History {
  class DeleteEntityCommand final : public EditorCommand {
    public:
      DeleteEntityCommand(const std::vector<Engine2D::Entity2D *> &entities, Engine2D::Scene *scene);

      void Execute() override;
      void Undo() override;
      std::string Name() const override;
    private:
      struct DeletedEntityData {
        int index, childIndex;
        bool wasActive;
        std::unique_ptr<Engine2D::Entity2D> entity;
        Engine2D::Entity2D *parent;
      };

      bool multipleEntitiesDeleted;
      Engine2D::Scene *scene;
      std::vector<Engine2D::Entity2D *> entities;
      std::vector<DeletedEntityData> deletedEntities;
  };
}

#endif //DELETE_ENTITY_COMMAND_HPP
