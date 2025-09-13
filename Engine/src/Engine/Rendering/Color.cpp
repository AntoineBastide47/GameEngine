//
// Color.cpp
// Author: Antoine Bastide
// Date: 06.09.2025
//

#include "Engine/Rendering/Color.hpp"

namespace Engine::Rendering {
  std::string Color::ToHex(const bool includeAlpha) const {
    const auto c = Clamped();
    const int ir = static_cast<int>(c.r * 255.0f + 0.5f);
    const int ig = static_cast<int>(c.g * 255.0f + 0.5f);
    const int ib = static_cast<int>(c.b * 255.0f + 0.5f);

    if (includeAlpha) {
      int ia = static_cast<int>(c.a * 255.0f + 0.5f);
      return std::format("#{:02X}{:02X}{:02X}{:02X}", ir, ig, ib, ia);
    }
    return std::format("#{:02X}{:02X}{:02X}", ir, ig, ib);
  }

  #if ENGINE_EDITOR
  bool Color::OnRenderInEditor(const std::string &name, const bool isConst, const bool readOnly) {
    RenderVariableName(name);

    float color[4] = {r, g, b, a};
    if (isConst) {
      ImGui::BeginDisabled(true);
      ImGui::ColorEdit4(name.c_str(), color);
      ImGui::EndDisabled();
      return false;
    }

    bool changed = false;
    ImGui::BeginDisabled(readOnly);
    if (ImGui::ColorEdit4(name.c_str(), color)) {
      changed = true;
      *this = Color{color};
    }
    ImGui::EndDisabled();
    return changed;
  }
  #endif
}
