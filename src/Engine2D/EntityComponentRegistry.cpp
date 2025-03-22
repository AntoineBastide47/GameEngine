//
// EntityComponentRegistry.cpp
// Author: Antoine Bastide
// Date: 08/03/2025
//

#include "Engine/EntityComponentRegistry.hpp"
#include "Engine2D/Component2D.hpp"

namespace Engine2D {
  entity_t EntityComponentRegistry::create() {
    entity_t entity;

    if (!unused_handles.empty()) {
      entity = unused_handles.back();
      unused_handles.pop_back();
    } else {
      entity = static_cast<entity_t>(handles.size());
      handles.push_back(entity);
    }

    return entity;
  }

  void EntityComponentRegistry::destroy(const entity_t entity) {
    const entity_t index = entity & ENTITY_INDEX_MASK;
    if (index >= handles.size())
      return;

    // Increment version and store back in entities list
    entity_t version = ((handles[index] & ENTITY_VERSION_MASK) >> 20) + 1;
    version = (version << 20) & ENTITY_VERSION_MASK;

    handles[index] = index | version;
    unused_handles.push_back(handles[index]);

    for (size_t i = 0; i < components.size(); ++i) {
      if (components[i]) {
        auto* baseStorage = static_cast<Engine::SparseSet<entity_t, Component2D>*>(components[i]);
        baseStorage->Remove(entity);
      }
    }
  }

  bool EntityComponentRegistry::is_valid(const entity_t entity) const {
    const entity_t index = entity & ENTITY_INDEX_MASK;
    if (index >= handles.size())
      return false;

    return handles[index] == entity;
  }

  size_t EntityComponentRegistry::index(const entity_t entity) {
    return entity & ENTITY_INDEX_MASK;
  }

  size_t EntityComponentRegistry::version(const entity_t entity) {
    return (entity & ENTITY_VERSION_MASK) >> 20;
  }

  std::vector<Behaviour *> EntityComponentRegistry::GetBehaviours(entity_t entity) {
    std::vector<Behaviour *> behaviours;

    /*
    for (auto &[type, sparseSet]: components)
      if (auto behaviour = reinterpret_cast<Behaviour*>(*sparseSet.TryGet(entity)))
        behaviours.push_back(behaviour);
*/

    return behaviours;
  }
}
