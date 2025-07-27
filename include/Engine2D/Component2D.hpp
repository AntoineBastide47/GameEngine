//
// Component2D.h
// Author: Antoine Bastide
// Date: 22/11/2024
//

#ifndef COMPONENT2D_H
#define COMPONENT2D_H

#include <memory>

#include "Engine/Reflection/Reflectable.hpp"
#include "Component2D.gen.hpp"

namespace Engine2D {
  class Entity2D;
  class Transform2D;

  /**
   * Component2D is the base class for all 2D components linked to a 2D Entity (it works like an interface in other languages).
   * If you need to create a component and want to link it to an Entity2D using Entity2D::AddComponent(), your component will
   * need to inherit from this class for it to work.
   */
  class Component2D : public Engine::Reflection::Reflectable {
    SERIALIZE_COMPONENT2D
      friend class Entity2D;
      friend class Transform2D;
    public:
      ~Component2D() override = default;

      /// Changes the active state of this component
      void SetActive(bool active);
      /// @returns True if this component and its parent entity are active, false if not
      [[nodiscard]] bool IsActive() const;

      /// @returns The entity this component is attached to
      [[nodiscard]] Entity2D *Entity() const;
      /// @returns The transform attached to the entity this component is attached to
      [[nodiscard]] Transform2D *Transform() const;
    protected:
      Component2D();
      /// Internal call to forward this component to the system that handles it
      virtual void forward() {}
      /// Internal call to recall this component to the system that handles it
      virtual void recall() {}
      /// Whether this component is active in the scene
      /// @warning Do not use directly, use the getter and setter instead as the component being active entails more that just this flag being true
      bool active;
    private:
      /// The entity this component is attached to
      Entity2D *entity;

      /// Sets the parent entity this component is attached to
      void setEntity(Entity2D *entity);
  };
}

#endif //COMPONENT2D_H
