//
// Behaviour.h
// Author: Antoine Bastide
// Date: 18/02/2025
//

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "Engine/Types/Ptr.hpp"
#include "Engine2D/Component2D.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Physics/Collider2D.hpp"
#include "Behaviour.gen.hpp"

namespace Engine2D {
  class Behaviour : public Component2D {
    SERIALIZE_BEHAVIOUR
    public:
      /// Called before the first update.
      virtual void OnInitialize() {}
      /// Called before the first update and after deserialization to bind input correctly.
      virtual void OnBindInput() {}
      /// Called once per frame.
      virtual void OnUpdate() {}
      /// Called once per physics update.
      virtual void OnFixedUpdate() {}
      /// Called when the entity is removed from the game or when the game quits.
      virtual void OnDestroy() {}

      /// Called when this entity first collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionEnter2D(const Engine::Ptr<Physics::Collider2D> &collider) {}
      /// Called while this entity collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionStay2D(const Engine::Ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity stops collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionExit2D(const Engine::Ptr<Physics::Collider2D> &collider) {}

      /// Called when this entity is first triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerEnter2D(const Engine::Ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity is first triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerStay2D(const Engine::Ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity is stops triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerExit2D(const Engine::Ptr<Physics::Collider2D> &collider) {}
    private:
      using Component2D::forward;
      using Component2D::recall;
  };
} // Engine2D

#endif //BEHAVIOUR_H
