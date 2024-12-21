//
// IComponent2D.h
// Author: Antoine Bastide
// Date: 22/11/2024
//

#ifndef COMPONENT2D_H
#define COMPONENT2D_H

#include <memory>

namespace Engine2D::Rendering {
  class ShapeRenderer;
}
namespace Engine2D {
  class Game2D;
  class Entity2D;
  class Transform2D;
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
      /// Weather or not this component is active in the scene
      bool active;

      Component2D();
      virtual ~Component2D();

      /// @returns The entity this component is attached to
      [[nodiscard]] std::shared_ptr<Entity2D> &Entity();
      /// @returns The transform attached to the entity this component is attached to
      [[nodiscard]] Transform2D *Transform() const;
    private:
      /// The entity this component is attached to
      std::shared_ptr<Entity2D> entity;
      /// The transform of the entity this component is attached to
      Transform2D *transform;
      /// Sets the parent entity this component is attached to
      void setEntity(const std::shared_ptr<Entity2D> &entity);
  };
}

#endif //COMPONENT2D_H
