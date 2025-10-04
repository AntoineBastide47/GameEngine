//
// CreateEntityCommand.hpp
// Author: Antoine Bastide
// Date: 17.08.2025
//

#ifndef CREATE_ENTITY_COMMAND_HPP
#define CREATE_ENTITY_COMMAND_HPP

#include <functional>

#include "Engine2D/Entity2D.hpp"
#include "History/EditorCommand.hpp"

namespace Engine2D {
  class Scene;
  class Entity2D;
}

namespace Editor::History {
  class CreateEntityCommand final : public EditorCommand {
    public:
      CreateEntityCommand(
        const Engine::Ptr<Engine2D::Scene> &scene, const std::vector<Engine::Ptr<Engine2D::Entity2D>> &parents,
        const std::function<void(Engine::Ptr<Engine2D::Entity2D>)> &createCallback,
        const std::function<void(Engine::Ptr<Engine2D::Entity2D>)> &deleteCallback, int insertPos = -1
      );

      void Execute() override;
      void Undo() override;
      std::string Name() const override;
    private:
      int insertPos;
      Engine::Ptr<Engine2D::Scene> scene;
      std::vector<Engine::Ptr<Engine2D::Entity2D>> parents;
      std::vector<Engine::Ptr<Engine2D::Entity2D>> createdEntities;
      std::unordered_map<Engine::Ptr<Engine2D::Entity2D>, int> indexes;
      std::function<void(Engine::Ptr<Engine2D::Entity2D>)> createCallback;
      std::function<void(Engine::Ptr<Engine2D::Entity2D>)> deleteCallback;
  };
}

#endif //CREATE_ENTITY_COMMAND_HPP
