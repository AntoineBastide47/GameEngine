//
// float01.cpp
// Author: Antoine Bastide
// Date: 14/12/2024
//

#include "Engine/Types/float01.hpp"
#include "Engine/Log.hpp"

namespace Engine {
  float01::float01() {
    this->value = 0;
  }

  float01::float01(const float value) {
    this->value = std::clamp(value, 0.0f, 1.0f);
  }

  float01 &float01::operator=(const float value) {
    this->value = std::clamp(value, 0.0f, 1.0f);
    return *this;
  }

  float01 &float01::operator+=(const float value) {
    return *this = this->value + value;
  }

  float01 &float01::operator-=(const float value) {
    return *this = this->value - value;
  }

  float01 &float01::operator*=(const float value) {
    return *this = this->value * value;
  }

  float01 &float01::operator/=(const float value) {
    if (value == 0.0f) {
      Log::Warning("float01 division by 0");
      return *this;
    }
    return *this = this->value / value;
  }

  #if ENGINE_EDITOR
  bool float01::OnRenderInEditor(const std::string &name, const bool isConst, const bool readOnly) {
    RenderVariableName(name);

    if (isConst) {
      ImGui::BeginDisabled(true);
      float temp = value;
      ImGui::DragFloat(("##" + name).c_str(), &temp);
      ImGui::EndDisabled();
      return false;
    }

    ImGui::BeginDisabled(readOnly);
    bool changed = false;
    float temp = value;
    if (ImGui::DragFloat(("##" + name).c_str(), &temp, 0.01f, 0, 1)) {
      value = temp;
      changed = true;
    }
    ImGui::EndDisabled();
    return changed;
  }
  #endif
}
