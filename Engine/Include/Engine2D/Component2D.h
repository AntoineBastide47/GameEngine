//
// IComponent2D.h
// Author: Antoine Bastide
// Date: 22/11/2024
//

#ifndef COMPONENT2D_H
#define COMPONENT2D_H

#include <memory>

namespace Engine2D {
  class Game2D;
  class Entity2D;
  class Transform2D;

  namespace Rendering {
    class ShapeRenderer;
  }
}

namespace Engine2D {
  /**
   * Component2D is the base class for all 2D components linked to a 2D Entity (it works like an interface in other languages).
   * If you need to create a component and want to link it to an Entity2D using Entity2D::AddComponent(), your component will
   * need to inherit from this class for it to work.
   */
  class Component2D : protected std::enable_shared_from_this<Component2D> {
    friend class Entity2D;
    friend class Game2D;
    public:
      Component2D();
      virtual ~Component2D();

      /// Changes the active state of this component
      void SetActive(bool active);
      /// @returns True if this component and its parent entity are active, false if not
      [[nodiscard]] bool IsActive() const;

      /// @returns The entity this component is attached to
      [[nodiscard]] std::shared_ptr<Entity2D> &Entity();
      /// @returns The transform attached to the entity this component is attached to
      [[nodiscard]] Transform2D *Transform() const;

      /// Called before the first update.
      virtual void OnInitialize() {}
      /// Called once per frame.
      virtual void OnUpdate() {}
      /// Called once per physics update.
      virtual void OnFixedUpdate() {}
      /// Called when the entity is removed from the game or when the game quits.
      virtual void OnDestroy() {}
    private:
      /// Whether this component is active in the scene
      bool active;
      /// The entity this component is attached to
      std::shared_ptr<Entity2D> entity;

      /// Sets the parent entity this component is attached to
      void setEntity(const std::shared_ptr<Entity2D> &entity);
  };
}

#endif //COMPONENT2D_H
