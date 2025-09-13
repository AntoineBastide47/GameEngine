//
// ICustomRenderInEditor.hpp
// Author: Antoine Bastide
// Date: 06.09.2025
//

#ifndef I_CUSTOM_EDITOR_HPP
#define I_CUSTOM_EDITOR_HPP

#include <imgui.h>
#include <string>

namespace Engine::Reflection {
  struct ICustomEditor {
    virtual ~ICustomEditor() = default;

    #if ENGINE_EDITOR
    void RenderDefaultEditor(bool readOnly = false);
    static void RenderVariableName(const std::string &name);
    virtual bool OnRenderInEditor(const std::string &name, bool isConst, bool readOnly) = 0;
    #endif
  };
}

#endif //I_CUSTOM_EDITOR_HPP
