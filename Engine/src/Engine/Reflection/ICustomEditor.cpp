//
// IEditorRendering.cpp
// Author: Antoine Bastide
// Date: 06.09.2025
//

#include "Engine/Reflection/ICustomEditor.hpp"
#include "Engine/Reflection/Reflectable.hpp"

namespace Engine::Reflection {
  #if ENGINE_EDITOR
  void ICustomEditor::
  RenderDefaultEditor(const bool readOnly) {
    if (const auto reflectable = dynamic_cast<Reflectable *>(this))
      reflectable->_e_renderInEditor(readOnly);
  }

  void ICustomEditor::RenderVariableName(const std::string &name) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", name.c_str());
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
  }
  #endif
}
