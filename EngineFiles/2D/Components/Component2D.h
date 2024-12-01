//
// IComponent2D.h
// Author: Antoine Bastide
// Date: 22/11/2024
//

#ifndef COMPONENT2D_H
#define COMPONENT2D_H

namespace Engine2D {
  class Transform2D;
}
namespace Engine2D {
  class Entity2D;
}

namespace Engine2D {
  /**
   * Component2D is the base class for all 2D components linked to a 2D Entity (it works like an interface in other languages).
   * If you need to create a component and want to link it to an Entity2D using Entity2D::AddComponent(), your component will
   * need to inherit from this class for it to work.
   */
  class Component2D {
    friend class Entity2D;
    public:
      /** @returns The entity this component is attached to */
      [[nodiscard]] Entity2D *Entity() const;
      /** @returns The transform attached to the entity this component is attached to */
      [[nodiscard]] Transform2D *Transform() const;
      virtual ~Component2D();
    protected:
      /**
       * Sets the entity this component is attached to
       * @note Only override this if it is absolutely necessary, if not, leave it as so
       */
      void SetEntity(Entity2D *entity);
    private:
      Entity2D *entity{};
      Transform2D *transform{};
  };
}

#endif //COMPONENT2D_H
