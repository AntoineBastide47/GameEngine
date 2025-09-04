//
// RenderInEditor.hpp
// Author: Antoine Bastide
// Date: 22.08.2025
//

#ifndef RENDER_IN_EDITOR_HPP
#define RENDER_IN_EDITOR_HPP

#include <imgui.h>
#include <map>
#include <string>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Reflection/Concepts.hpp"

namespace Engine::Reflection {
  template<typename T> static bool _e_renderInEditorImpl(T &, const std::string &, bool readOnly = false);

  struct StringResizeData {
    std::string &str;
    int maxSize;

    explicit StringResizeData(std::string &str, const int maxSize)
      : str(str), maxSize(maxSize) {}
  };

  static int StringResizeCallback(ImGuiInputTextCallbackData *data) {
    const StringResizeData *userData = static_cast<StringResizeData *>(data->UserData);

    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
      std::string &str = userData->str;

      // Calculate new size (with some extra space)
      const size_t newSize = std::min(data->BufTextLen + 256, userData->maxSize);

      // Resize the string
      str.resize(newSize);

      // Update ImGui's buffer pointer
      data->Buf = str.data();
      data->BufSize = static_cast<int>(str.capacity());
    }
    return 0;
  }

  inline bool InputText(const char *label, std::string &str, const int maxSize = 1024 * 1024) {
    // Ensure minimum capacity
    if (str.capacity() < 256)
      str.reserve(256);
    str.resize(str.capacity() - 1);

    StringResizeData userData{str, maxSize};
    const bool changed = ImGui::InputText(
      label, str.data(), str.capacity(),
      ImGuiInputTextFlags_CallbackResize,
      StringResizeCallback, &userData
    );

    // Trim to actual content
    str.resize(strlen(str.data()));
    return changed;
  }

  template<IsNumber T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    ImGui::BeginDisabled(readOnly);
    bool changed = false;
    if constexpr (std::is_floating_point_v<T>) {
      float temp = static_cast<float>(data);
      if (ImGui::DragFloat(("##" + name).c_str(), &temp)) {
        data = static_cast<T>(temp);
        changed = true;
      }
    } else {
      int temp = static_cast<int>(data);
      if (ImGui::DragInt(("##" + name).c_str(), &temp)) {
        data = static_cast<T>(temp);
        changed = true;
      }
    }
    ImGui::EndDisabled();
    return changed;
  }

  template<IsOwningString T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    ImGui::BeginDisabled(readOnly);
    const bool changed = InputText(("##" + name).c_str(), data, 256);
    ImGui::EndDisabled();
    return changed;
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, const char *>
  static bool _e_renderInEditor(T &data, const char *, const bool) {
    ImGui::Text("%s", data ? data : "(null)");
    return false;
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, std::string_view>
  static bool _e_renderInEditor(const T &data, const char *, const bool) {
    ImGui::Text("%.*s", static_cast<int>(data.size()), data.data() ? data.data() : "(empty)");
    return false;
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, bool>
  static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    ImGui::BeginDisabled(readOnly);
    bool temp = data, changed = false;
    if (ImGui::Checkbox(("##" + name).c_str(), &temp)) {
      data = temp;
      changed = true;
    }
    ImGui::EndDisabled();
    return changed;
  }

  template<IsContainer T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Header with foldout arrow and size info
    const size_t size = std::distance(std::begin(data), std::end(data));
    const std::string headerText = std::to_string(size);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = ImGui::TreeNodeEx(
      name.c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_SpanAllColumns
    );

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Add/Remove buttons for dynamic containers
    ImGui::BeginDisabled(readOnly);
    if constexpr (requires { data.push_back(typename T::value_type{}); data.pop_back(); }) {
      ImGui::SameLine();
      if (ImGui::SmallButton("+")) {
        data.push_back(typename T::value_type{});
        changed = true;
      }

      ImGui::BeginDisabled(data.empty());
      ImGui::SameLine();
      if (ImGui::SmallButton("-")) {
        data.pop_back();
        changed = true;
      }
      ImGui::EndDisabled();
    }
    ImGui::EndDisabled();

    if (opened) {
      // Render each element
      size_t index = 0;
      for (auto &element: data) {
        ImGui::PushID(static_cast<int>(index));

        // Recursively render the element
        if (_e_renderInEditorImpl(element, "Element " + std::to_string(index), readOnly))
          changed = true;

        ImGui::PopID();
        ++index;
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
    return changed;
  }

  template<IsMap T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Header with foldout arrow and size info
    const size_t size = data.size();
    const std::string headerText = std::to_string(size);

    constexpr int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                          ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAllColumns;
    ImGui::PushID(("##" + name).c_str());
    const bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Add/Remove buttons for dynamic containers
    ImGui::BeginDisabled(readOnly);
    if constexpr (requires {
      data.emplace(typename T::key_type{}, typename T::mapped_type{}); data.erase(data.begin());
    }) {
      ImGui::SameLine();
      if (ImGui::SmallButton("+")) {
        ImGui::OpenPopup("AddMapEntry");
      }

      static typename T::key_type newKey{};

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
      ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5);
      if (ImGui::BeginPopup("AddMapEntry")) {
        bool shouldAdd = false;

        if (ImGui::BeginTable((name + "Table").c_str(), 2, ImGuiTableFlags_SizingFixedFit)) {
          ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 75);
          ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 125);

          _e_renderInEditorImpl(newKey, " New Key");

          ImGui::EndTable();
        }

        ImGui::SetCursorPosX(20);
        if (ImGui::Button("Add", ImVec2(60, 0))) {
          if (data.find(newKey) == data.end()) {
            data[newKey] = typename T::mapped_type{};
            changed = true;
            shouldAdd = true;
          }
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(120);
        if (ImGui::Button("Cancel", ImVec2(60, 0))) {
          shouldAdd = false;
          ImGui::CloseCurrentPopup();
        }

        if (shouldAdd) {
          newKey = typename T::key_type{};
          ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
      }
      ImGui::PopStyleVar(3);

      ImGui::BeginDisabled(data.empty());
      ImGui::SameLine();
      if (ImGui::SmallButton("-")) {
        data.erase(std::prev(data.end()));
        changed = true;
      }
      ImGui::EndDisabled();
    }
    ImGui::EndDisabled();

    if (opened) {
      size_t index = 0;
      for (auto &[k, v]: data) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::PushID(("##" + name + "_element" + std::to_string(index)).c_str());
        if (ImGui::TreeNodeEx(("Element " + std::to_string(index)).c_str(), flags)) {
          ImGui::PushID(("##" + name + "_key" + std::to_string(index)).c_str());
          _e_renderInEditorImpl(const_cast<typename T::key_type &>(k), "Key", true);
          ImGui::PopID();

          ImGui::PushID(("##" + name + "_value" + std::to_string(index)).c_str());
          if (_e_renderInEditorImpl(v, "Value", readOnly))
            changed = true;
          ImGui::PopID();

          ImGui::TreePop();
        }

        ImGui::PopID();
        ++index;

        ImGui::TableNextColumn();
        ImGui::Text("");
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
    return changed;
  }

  template<IsPair T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = ImGui::TreeNodeEx(
      name.c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_SpanAllColumns
    );

    if (opened) {
      // Render each element
      ImGui::PushID(("##" + name + "_first").c_str());
      if (_e_renderInEditorImpl(data.first, "First", readOnly))
        changed = true;
      ImGui::PopID();

      ImGui::PushID(("##" + name + "_second").c_str());
      if (_e_renderInEditorImpl(data.second, "Second", readOnly))
        changed = true;
      ImGui::PopID();

      ImGui::TreePop();
    }

    ImGui::PopID();
    return changed;
  }

  template<typename... Ts> static bool _e_renderInEditor(
    std::tuple<Ts...> &data, const std::string &name, const bool readOnly
  ) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = ImGui::TreeNodeEx(
      name.c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_SpanAllColumns
    );

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    if (opened) {
      // Helper lambda to render each tuple element recursively
      auto renderElement = [&]<size_t Index>(std::integral_constant<size_t, Index>) {
        if constexpr (Index < sizeof...(Ts)) {
          ImGui::PushID(static_cast<int>(Index));

          if (auto &element = std::get<Index>(data);
            _e_renderInEditorImpl(element, "Element " + std::to_string(Index), readOnly)) {
            changed = true;
          }

          ImGui::PopID();
        }
      };

      // Render each tuple element using fold expression
      [&]<size_t... Indices>(std::index_sequence<Indices...>) {
        (renderElement(std::integral_constant<size_t, Indices>{}), ...);
      }(std::make_index_sequence<sizeof...(Ts)>{});

      ImGui::TreePop();
    }

    ImGui::PopID();
    return changed;
  }

  template<IsSmartPtr T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    return false;
  }

  template<IsReflectable T> static bool _e_renderInEditor(T &data, const std::string &, const bool readOnly) {
    return data._e_renderInEditor(readOnly);
  }

  template<IsReflectablePointer T> static bool _e_renderInEditor(
    T *data, const std::string &name, const bool readOnly
  ) {
    return false;
  }

  template<IsEnum T> static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly) {
    if (!ReflectionFactory::EnumRegistered<T>())
      return false;

    bool changed = false;

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", name.c_str());
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::BeginDisabled(readOnly);

    const auto &values = ReflectionFactory::GetEnumValues<T>();
    size_t currentIndex = 0;
    const std::string currentValue = ReflectionFactory::EnumValueToString(data);
    for (size_t i = 0; i < values.size(); ++i) {
      if (values[i] == currentValue) {
        currentIndex = i;
        break;
      }
    }

    if (ImGui::BeginCombo(
      ("##enum_" + name).c_str(), currentIndex < values.size() ? values[currentIndex].c_str() : "Unknown"
    )) {
      for (size_t i = 0; i < values.size(); ++i) {
        const bool isSelected = i == currentIndex;
        if (ImGui::Selectable(values[i].c_str(), isSelected) && i != currentIndex)
          if (const auto potentialVal = ReflectionFactory::StringToEnumValue<T>(values[i])) {
            data = *potentialVal;
            changed = true;
          }

        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::EndDisabled();

    return changed;
  }

  template<glm::length_t N, typename T, glm::qualifier Q> requires (2 <= N && N <= 4)
  static bool _e_renderInEditor(glm::vec<N, T, Q> &data, const std::string &name, const bool readOnly) {
    using Type = std::remove_cvref_t<T>;

    ImGui::BeginDisabled(readOnly);
    bool changed = false;

    if constexpr (std::is_floating_point_v<Type>) {
      if constexpr (N == 2) {
        changed = ImGui::DragFloat2(("##" + name).c_str(), glm::value_ptr(data));
      } else if constexpr (N == 3) {
        changed = ImGui::DragFloat3(("##" + name).c_str(), glm::value_ptr(data));
      } else {
        changed = ImGui::DragFloat4(("##" + name).c_str(), glm::value_ptr(data));
      }
    } else if constexpr (std::is_same_v<Type, bool>) {
      static constexpr std::array labels = {"X:", " Y:", " Z:", " W:"};
      for (glm::length_t i = 0; i < N; ++i) {
        if (i > 0)
          ImGui::SameLine();

        ImGui::Text("%s", labels[i]);
        ImGui::SameLine();

        bool temp = data[i];
        std::string checkboxId = "##" + name + "_" + std::to_string(i);
        if (ImGui::Checkbox(checkboxId.c_str(), &temp)) {
          data[i] = temp;
          changed = true;
        }
      }
    } else {
      if constexpr (N == 2) {
        int temp[2] = {static_cast<int>(data[0]), static_cast<int>(data[1])};
        if (ImGui::DragInt2(("##" + name).c_str(), temp)) {
          data[0] = static_cast<T>(temp[0]);
          data[1] = static_cast<T>(temp[1]);
          changed = true;
        }
      } else if constexpr (N == 3) {
        int temp[3] = {static_cast<int>(data[0]), static_cast<int>(data[1]), static_cast<int>(data[2])};
        if (ImGui::DragInt3(("##" + name).c_str(), temp)) {
          data[0] = static_cast<T>(temp[0]);
          data[1] = static_cast<T>(temp[1]);
          data[2] = static_cast<T>(temp[2]);
          changed = true;
        }
      } else {
        int temp[4] = {
          static_cast<int>(data[0]), static_cast<int>(data[1]),
          static_cast<int>(data[2]), static_cast<int>(data[3])
        };
        if (ImGui::DragInt4(("##" + name).c_str(), temp)) {
          data[0] = static_cast<T>(temp[0]);
          data[1] = static_cast<T>(temp[1]);
          data[2] = static_cast<T>(temp[2]);
          data[3] = static_cast<T>(temp[3]);
          changed = true;
        }
      }
    }

    ImGui::EndDisabled();
    return changed;
  }

  template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q> requires (
    2 <= C && C <= 4 && 2 <= R && R <= 4)
  static bool _e_renderInEditor(glm::mat<C, R, T, Q> &data, const std::string &name, const bool readOnly) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = ImGui::TreeNodeEx(
      name.c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAllColumns
    );

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::Text("[%dx%d]", C, R);

    ImGui::BeginDisabled(readOnly);
    if (opened) {
      for (glm::length_t col = 0; col < C; ++col) {
        for (glm::length_t row = 0; row < R; ++row) {
          ImGui::PushID(("##" + name + "_" + std::to_string(col * R + row)).c_str());

          const std::string elementLabel = "[" + std::to_string(row) + "][" + std::to_string(col) + "]";

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text("%s", elementLabel.c_str());

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1);

          if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>) {
            float temp = data[col][row];
            if (ImGui::DragFloat(("##" + elementLabel).c_str(), &temp)) {
              data[col][row] = temp;
              changed = true;
            }
          } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, double>) {
            float temp = static_cast<float>(data[col][row]);
            if (ImGui::DragFloat(("##" + elementLabel).c_str(), &temp)) {
              data[col][row] = static_cast<double>(temp);
              changed = true;
            }
          } else if constexpr (std::is_integral_v<T>) {
            int temp = static_cast<int>(data[col][row]);
            if (ImGui::DragInt(("##" + elementLabel).c_str(), &temp)) {
              data[col][row] = static_cast<T>(temp);
              changed = true;
            }
          }

          ImGui::PopID();
        }
      }

      ImGui::TreePop();
    }
    ImGui::EndDisabled();

    ImGui::PopID();
    return changed;
  }

  template<size_t N> constexpr std::array<std::string_view, N> splitToArray(const std::string_view str) {
    std::array<std::string_view, N> result{};

    size_t start = 0;
    size_t index = 0;
    size_t angleBracketDepth = 0;
    for (size_t c = 0; c < str.size(); ++c) {
      if (str[c] == ',' && angleBracketDepth == 0 && c < str.size() - 1 && str[c + 1] == ' ') {
        result[index++] = str.substr(start, c - start);
        start = c + 2;
      } else if (str[c] == '<')
        angleBracketDepth++;
      else if (str[c] == '>')
        angleBracketDepth--;
    }

    if (index < N && start < str.length())
      result[index] = str.substr(start);

    return result;
  }

  template<typename... Ts> static constexpr std::array<std::string_view, sizeof...(Ts)> _e_getVariantTypes() {
    constexpr std::string_view funcSig = ENGINE_FUNC_SIG;

    #if defined(__clang__)
    constexpr size_t start = funcSig.find("[Ts = <") + 7;
    constexpr size_t end = funcSig.find(">]", start);
    #elif defined(__GNUC__)
    constexpr auto indices = []() {
      std::string_view fs = __PRETTY_FUNCTION__;
      size_t s = fs.find("[with Ts = {");
      if (s == std::string_view::npos) {
        s = fs.find("[Ts = {");
        if (s == std::string_view::npos)
          return std::pair<size_t, size_t>{0, 0};
        s += 7;
      } else {
        s += 12;
      }
      size_t e = fs.find("}]", s);
      return std::pair{s, e};
    }();
    constexpr size_t start = indices.first;
    constexpr size_t end = indices.second;
    #else
    constexpr auto start = funcSig.find_first_of("<");
    constexpr auto end = funcSig.find_last_of(">");
    #endif

    return splitToArray<sizeof...(Ts)>(funcSig.substr(start, end - start));
  }

  template<typename... Ts> static bool _e_renderInEditor(
    std::variant<Ts...> &data, const std::string &name, const bool readOnly
  ) {
    static constexpr std::array<std::string_view, sizeof...(Ts)> arr = _e_getVariantTypes<Ts...>();

    bool changed = false;

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    ImGui::AlignTextToFramePadding();
    const bool opened = ImGui::TreeNodeEx(
      name.c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_FramePadding
    );

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Type selector dropdown
    const size_t currentIndex = data.index();

    ImGui::BeginDisabled(readOnly);
    if (ImGui::BeginCombo(
      ("##type_" + name).c_str(), currentIndex < arr.size() ? std::string(arr[currentIndex]).c_str() : "Unknown"
    )) {
      for (size_t i = 0; i < sizeof...(Ts); ++i) {
        const bool isSelected = i == currentIndex;
        if (ImGui::Selectable(std::string(arr[i]).c_str(), isSelected)) {
          if (i != currentIndex) {
            // Change variant type using a compile-time index sequence
            [&]<size_t... Indices>(std::index_sequence<Indices...>) {
              auto changeToType = [&]<size_t Index>() {
                if (Index == i) {
                  using TargetType = std::variant_alternative_t<Index, std::variant<Ts...>>;
                  data = TargetType{};
                  changed = true;
                }
              };
              (changeToType.template operator()<Indices>(), ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
          }
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    ImGui::EndDisabled();

    if (opened) {
      // Render the current variant data
      std::visit(
        [&](auto &value) {
          ImGui::PushID(("variant_data_" + name).c_str());
          if (_e_renderInEditorImpl(value, "Value", readOnly)) {
            changed = true;
          }
          ImGui::PopID();
        }, data
      );

      ImGui::TreePop();
    }

    ImGui::PopID();

    return changed;
  }

  template<typename T> concept HasFreeRenderInEditor = requires(T &t, const std::string &name, const bool readOnly) {
    { _e_renderInEditor(t, name, readOnly) } -> std::same_as<bool>;
  };

  template<typename T> concept HasMemberRenderInEditor = requires(T &obj, const bool readOnly) {
    { obj._e_renderInEditor(readOnly) } -> std::same_as<bool>;
  };

  template<typename T> concept HasRenderInEditorFunction = HasFreeRenderInEditor<T> || HasMemberRenderInEditor<T>;

  template<typename T> static bool _e_renderInEditorImpl(T &data, const std::string &name, const bool readOnly) {
    if constexpr (HasRenderInEditorFunction<T>) {
      if constexpr (!IsEnum<T>)
        ImGui::TableNextRow();
      if constexpr (IsNonRecursive<T>) {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", name.c_str());
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
      }

      if (_e_renderInEditor(data, name, readOnly))
        return true;
    }
    return false;
  }
}

#endif //RENDER_IN_EDITOR_HPP
