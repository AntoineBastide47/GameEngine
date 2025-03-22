//
// Behaviour.h
// Author: Antoine Bastide
// Date: 18/02/2025
//

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <memory>

#include "Engine2D/Component2D.hpp"

namespace Engine2D {
  namespace Physics {
    class Collider2D;
  }
  class Behaviour : public Component2D {
    public:
      /// Called before the first update.
      virtual void OnInitialize() {}
      /// Called once per frame.
      virtual void OnUpdate() {}
      /// Called once per physics update.
      virtual void OnFixedUpdate() {}
      /// Used to draw debug information to the screen
      virtual void OnDrawGizmos2D() {}
      /// Called when the entity is removed from the game or when the game quits.
      virtual void OnDestroy() {}

      /// Called when this entity first collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionEnter2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called while this entity collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionStay2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity stops collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionExit2D(const std::shared_ptr<Physics::Collider2D> &collider) {}

      /// Called when this entity is first triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerEnter2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity is first triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerStay2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity is stops triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerExit2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
  };
} // Engine2D

#endif //BEHAVIOUR_H
