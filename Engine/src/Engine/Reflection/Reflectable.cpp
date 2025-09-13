//
// Reflectable.cpp
// Author: Antoine Bastide
// Date: 13.09.2025
//

#include "Engine/Reflection/Reflectable.hpp"

namespace Engine::Reflection {
  #if ENGINE_EDITOR
  bool Reflectable::_e_renderInEditorImpl(const bool readOnly, const std::string &name) {
    _e_currentEntityName = name;
    return _e_renderInEditor(readOnly);
  }
  #endif
}