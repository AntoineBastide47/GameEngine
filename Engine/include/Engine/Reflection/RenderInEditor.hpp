//
// RenderInEditor.hpp
// Author: Antoine Bastide
// Date: 22.08.2025
//

#ifndef RENDER_IN_EDITOR_HPP
#define RENDER_IN_EDITOR_HPP

#if ENGINE_EDITOR

#include <imgui.h>
#include <map>
#include <string>
#include <glm/gtc/type_ptr.hpp>

#include "../../../../Editor/include/History/CommandHistory.hpp"
#include "Engine/Macros/Utils.hpp"
#include "Engine/Reflection/Concepts.hpp"
#include "Engine/Reflection/Container.hpp"
#include "Engine/Reflection/ICustomEditor.hpp"
#include "Engine/Reflection/ReflectionFactory.hpp"

namespace Engine2D::Physics {
  class BoxCollider2D;
}

namespace Engine::Reflection {
  inline std::string _e_currentEntityName = "";
  inline bool _e_createEditorCommand = true;

  template<typename T> static bool _e_renderInEditorImpl(
    T &, const std::string &, bool readOnly = false, const std::string &id = ""
  );

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

  inline bool InputText(
    const std::string &name, std::string &str, const int maxSize = 1024 * 1024, const bool rename = false
  ) {
    std::string oldValue = str;
    std::string copy = str;

    // Ensure minimum capacity
    if (copy.capacity() < 256)
      copy.reserve(256);
    copy.resize(copy.capacity() - 1);

    StringResizeData userData{copy, maxSize};
    const bool changed = ImGui::InputText(
      ("##" + name).c_str(), copy.data(), copy.capacity(),
      ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue,
      StringResizeCallback, &userData
    );
    copy.resize(strlen(copy.data()));

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      str = oldValue;
      return true;
    }

    if (changed || (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) || ImGui::IsItemDeactivated()) {
      if (oldValue != copy) {
        if (_e_createEditorCommand) {
          Editor::History::CommandHistory::Create(
            Editor::History::MakeLambdaCommand(
              rename ? "Renamed " + oldValue + " to " + copy : "Changed " + name + " in " + _e_currentEntityName,
              [&str, copy] {
                str = copy;
              },
              [&str, oldValue] {
                str = oldValue;
              }
            )
          );
        }

        return true;
      }
    }

    str = oldValue;
    return false;
  }

  template<typename T> struct DragTypeTraits {
    static constexpr bool isArray = false;
    using ValueType = std::remove_const_t<T>;
  };

  template<typename T, size_t N> struct DragTypeTraits<T[N]> {
    static constexpr bool isArray = true;
    using ValueType = T;
  };

  template<typename> struct IsGlmVec : std::false_type {};

  template<glm::length_t N, typename T, glm::qualifier Q> struct IsGlmVec<glm::vec<N, T, Q>> : std::true_type {};

  template<typename T> inline constexpr bool IsGlmVec_v = IsGlmVec<std::remove_cv_t<T>>::value;

  template<typename T> bool ImGuiDrag(
    const char *label, T *v, const float v_speed = 1.0f, const T *v_min = nullptr, const T *v_max = nullptr
  ) {
    ImGuiDataType dataType = ImGuiDataType_COUNT;

    // Handle floating point types
    if constexpr (std::is_floating_point_v<T>) {
      if constexpr (sizeof(T) == sizeof(float))
        dataType = ImGuiDataType_Float;
      else if constexpr (sizeof(T) == sizeof(double))
        dataType = ImGuiDataType_Double;
    }
    // Handle integral types based on size and signedness
    else if constexpr (std::is_integral_v<T>) {
      if constexpr (std::is_signed_v<T>) {
        if constexpr (sizeof(T) == 1)
          dataType = ImGuiDataType_S8;
        else if constexpr (sizeof(T) == 2)
          dataType = ImGuiDataType_S16;
        else if constexpr (sizeof(T) == 4)
          dataType = ImGuiDataType_S32;
        else if constexpr (sizeof(T) == 8)
          dataType = ImGuiDataType_S64;
      } else {
        if constexpr (sizeof(T) == 1)
          dataType = ImGuiDataType_U8;
        else if constexpr (sizeof(T) == 2)
          dataType = ImGuiDataType_U16;
        else if constexpr (sizeof(T) == 4)
          dataType = ImGuiDataType_U32;
        else if constexpr (sizeof(T) == 8)
          dataType = ImGuiDataType_U64;
      }
    }

    if (dataType != ImGuiDataType_COUNT) {
      T min_val = v_min
                    ? *v_min
                    : std::is_floating_point_v<T>
                        ? std::numeric_limits<T>::lowest()
                        : std::numeric_limits<T>::min();
      T max_val = v_max ? *v_max : std::numeric_limits<T>::max();

      return ImGui::DragScalar(label, dataType, v, v_speed, &min_val, &max_val);
    }

    return false;
  }

  template<typename T, size_t N> bool ImGuiDrag(
    const char *label, T (*v)[N], const float v_speed = 1.0f,
    const T *v_min = nullptr, const T *v_max = nullptr
  ) {
    ImGuiDataType dataType = ImGuiDataType_COUNT;

    if constexpr (std::is_floating_point_v<T>) {
      if constexpr (sizeof(T) == sizeof(float))
        dataType = ImGuiDataType_Float;
      else if constexpr (sizeof(T) == sizeof(double))
        dataType = ImGuiDataType_Double;
    } else if constexpr (std::is_integral_v<T>) {
      if constexpr (std::is_signed_v<T>) {
        if constexpr (sizeof(T) == 1)
          dataType = ImGuiDataType_S8;
        else if constexpr (sizeof(T) == 2)
          dataType = ImGuiDataType_S16;
        else if constexpr (sizeof(T) == 4)
          dataType = ImGuiDataType_S32;
        else if constexpr (sizeof(T) == 8)
          dataType = ImGuiDataType_S64;
      } else {
        if constexpr (sizeof(T) == 1)
          dataType = ImGuiDataType_U8;
        else if constexpr (sizeof(T) == 2)
          dataType = ImGuiDataType_U16;
        else if constexpr (sizeof(T) == 4)
          dataType = ImGuiDataType_U32;
        else if constexpr (sizeof(T) == 8)
          dataType = ImGuiDataType_U64;
      }
    }

    if (dataType != ImGuiDataType_COUNT && N >= 2 && N <= 4) {
      T min_val = v_min
                    ? *v_min
                    : std::is_floating_point_v<T>
                        ? std::numeric_limits<T>::lowest()
                        : std::numeric_limits<T>::min();
      T max_val = v_max ? *v_max : std::numeric_limits<T>::max();

      return ImGui::DragScalarN(label, dataType, *v, static_cast<int>(N), v_speed, &min_val, &max_val);
    }

    return false;
  }

  template<glm::length_t N, typename T, glm::qualifier Q> bool ImGuiDrag(
    const char *label, glm::vec<N, T, Q> *v, const float v_speed = 1.0f,
    const T *v_min = nullptr, const T *v_max = nullptr
  ) {
    ImGuiDataType dataType = ImGuiDataType_COUNT;

    if constexpr (std::is_floating_point_v<T>) {
      if constexpr (sizeof(T) == sizeof(float))
        dataType = ImGuiDataType_Float;
      else if constexpr (sizeof(T) == sizeof(double))
        dataType = ImGuiDataType_Double;
    } else if constexpr (std::is_integral_v<T>) {
      if constexpr (std::is_signed_v<T>) {
        if constexpr (sizeof(T) == 1)
          dataType = ImGuiDataType_S8;
        else if constexpr (sizeof(T) == 2)
          dataType = ImGuiDataType_S16;
        else if constexpr (sizeof(T) == 4)
          dataType = ImGuiDataType_S32;
        else if constexpr (sizeof(T) == 8)
          dataType = ImGuiDataType_S64;
      } else {
        if constexpr (sizeof(T) == 1)
          dataType = ImGuiDataType_U8;
        else if constexpr (sizeof(T) == 2)
          dataType = ImGuiDataType_U16;
        else if constexpr (sizeof(T) == 4)
          dataType = ImGuiDataType_U32;
        else if constexpr (sizeof(T) == 8)
          dataType = ImGuiDataType_U64;
      }
    }

    if (dataType != ImGuiDataType_COUNT && N >= 2 && N <= 4) {
      T min_val = v_min
                    ? *v_min
                    : std::is_floating_point_v<T>
                        ? std::numeric_limits<T>::lowest()
                        : std::numeric_limits<T>::min();
      T max_val = v_max ? *v_max : std::numeric_limits<T>::max();

      return ImGui::DragScalarN(label, dataType, glm::value_ptr(*v), N, v_speed, &min_val, &max_val);
    }

    return false;
  }

  template<typename T> struct DragState {
    T currentValue;
    T beforeDragValue;
    bool isDragging = false;
    bool forceChange = true;
  };

  template<typename T> static std::unordered_map<std::string, DragState<T>> &GetDragStateMap() {
    static std::unordered_map<std::string, DragState<T>> stateMap;
    return stateMap;
  }

  template<typename T> static bool DragWithHistory(
    T &data, const std::string &name, const float v_speed = 1.0f, const bool readOnly = false
  ) {
    constexpr bool isConst = IsConst<T>;
    using ValueType = std::remove_const_t<T>;

    ImGui::BeginDisabled(readOnly || isConst);
    bool changed = false;

    if constexpr (!isConst) {
      auto &stateMap = GetDragStateMap<ValueType>();
      const std::string stateKey = _e_currentEntityName + "_" + name;
      auto &state = stateMap[stateKey];

      ImGuiDrag(("##" + name).c_str(), &state.currentValue, v_speed);

      if (!state.isDragging) {
        if constexpr (DragTypeTraits<ValueType>::isArray)
          std::copy(std::begin(data), std::end(data), std::begin(state.currentValue));
        else
          state.currentValue = data;
      }

      if (state.forceChange) {
        state.forceChange = false;
        changed = true;
      }

      // Handle drag start
      if (ImGui::IsItemActivated()) {
        state.isDragging = true;
        if constexpr (DragTypeTraits<ValueType>::isArray)
          std::copy(std::begin(data), std::end(data), std::begin(state.beforeDragValue));
        else
          state.beforeDragValue = data;
      }

      // Handle drag end - create undo/redo command
      if (ImGui::IsItemDeactivated() && state.isDragging) {
        state.isDragging = false;

        bool valuesChanged = false;
        if constexpr (DragTypeTraits<ValueType>::isArray) {
          valuesChanged = !std::equal(
            std::begin(state.currentValue),
            std::end(state.currentValue),
            std::begin(state.beforeDragValue)
          );
        } else {
          if constexpr (IsGlmVec_v<ValueType>)
            valuesChanged = glm::any(glm::notEqual(state.currentValue, state.beforeDragValue));
          else
            valuesChanged = state.currentValue != state.beforeDragValue;
        }

        if (valuesChanged) {
          if (_e_createEditorCommand)
            Editor::History::CommandHistory::Create(
              Editor::History::MakeLambdaCommand(
                "Changed " + name + " in " + _e_currentEntityName,
                [&data, &stateMap, stateKey, newValue = state.currentValue] {
                  if constexpr (DragTypeTraits<ValueType>::isArray)
                    std::copy(std::begin(newValue), std::end(newValue), std::begin(data));
                  else
                    data = newValue;
                  stateMap.at(stateKey).forceChange = true;
                },
                [&data, &stateMap, stateKey, oldValue = state.beforeDragValue] {
                  if constexpr (DragTypeTraits<ValueType>::isArray)
                    std::copy(std::begin(oldValue), std::end(oldValue), std::begin(data));
                  else
                    data = oldValue;
                  stateMap.at(stateKey).forceChange = true;
                }
              )
            );
          else {
            if constexpr (DragTypeTraits<ValueType>::isArray)
              std::copy(std::begin(state.currentValue), std::end(state.currentValue), std::begin(data));
            else
              data = state.currentValue;
          }

          changed = true;
        }
      }

      // Update data while dragging
      if (state.isDragging) {
        if constexpr (DragTypeTraits<ValueType>::isArray)
          std::copy(std::begin(state.currentValue), std::end(state.currentValue), std::begin(data));
        else
          data = state.currentValue;
        changed = true;
      }
    } else {
      ValueType temp;
      if constexpr (DragTypeTraits<ValueType>::isArray)
        std::copy(std::begin(data), std::end(data), std::begin(temp));
      else
        temp = static_cast<ValueType>(data);

      ImGuiDrag(("##" + name).c_str(), &temp, v_speed);
    }

    ImGui::EndDisabled();
    return changed;
  }

  template<IsNumber T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string &id = ""
  ) {
    ImGui::BeginDisabled(readOnly || IsConst<T>);
    const bool changed = DragWithHistory(data, id.empty() ? name : id);
    ImGui::EndDisabled();
    return changed;
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, std::string>
  static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly, const std::string & = "") {
    if constexpr (std::is_const_v<T>) {
      ImGui::BeginDisabled(true);
      ImGui::Text("%s", data.c_str());
      ImGui::EndDisabled();
      return false;
    } else {
      ImGui::BeginDisabled(readOnly);
      const bool changed = InputText(name, data, 256); // TODO: integrate command
      ImGui::EndDisabled();
      return changed;
    }
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, const char *>
  static bool _e_renderInEditor(T &data, const std::string &, const bool, const std::string & = "") {
    ImGui::Text("%s", data ? data : "(null)");
    return false;
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, std::string_view>
  static bool _e_renderInEditor(T &data, const std::string &, const bool, const std::string & = "") {
    ImGui::Text("%.*s", static_cast<int>(data.size()), data.data() ? data.data() : "(empty)");
    return false;
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, bool>
  static bool _e_renderInEditor(T &data, const std::string &name, const bool readOnly, const std::string &id = "") {
    constexpr bool isConst = IsConst<T>;
    ImGui::BeginDisabled(readOnly || isConst);
    bool temp = data, changed = false;
    ImGui::Checkbox(("##" + (id.empty() ? name : id)).c_str(), &temp);
    if constexpr (!isConst) {
      if (ImGui::IsItemDeactivated() && temp != data) {
        changed = true;

        if (_e_createEditorCommand)
          Editor::History::CommandHistory::Create(
            Editor::History::MakeLambdaCommand(
              "Toggled " + (id.empty() ? name : id) + " in " + _e_currentEntityName,
              [&data, temp] {
                data = temp;
              },
              [&data, oldData = data] {
                data = oldData;
              }
            )
          );
        else
          data = temp;
      }
    }
    ImGui::EndDisabled();
    return changed;
  }

  // ReSharper disable once CppDFAConstantParameter
  static bool _e_renderTreeNode(const std::string &name, const int extraFlags = 0) {
    constexpr auto flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAllColumns |
        ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
    return ImGui::TreeNodeEx(name.c_str(), flags | extraFlags);
  }

  template<typename T, size_t N> static bool _e_renderInEditor(
    T (&data)[N], const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Header with foldout arrow and size info
    ImGui::PushID(("##" + name).c_str());
    const bool opened = _e_renderTreeNode(name);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::Text("[%zu]", N);

    if (opened) {
      for (size_t i = 0; i < N; ++i) {
        ImGui::PushID(static_cast<int>(i));

        if (const std::string Id = "Element " + std::to_string(i);
          _e_renderInEditorImpl(data[i], Id, readOnly, name + '[' + std::to_string(i) + ']'))
          changed = true;

        ImGui::PopID();
      }
    }

    ImGui::PopID();
    return changed;
  }

  template<IsContainer T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    // Header with foldout arrow and size info
    const size_t size = std::distance(std::begin(data), std::end(data));
    const std::string headerText = std::to_string(size);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = _e_renderTreeNode(name, ImGuiTreeNodeFlags_AllowItemOverlap);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::Text("[%zu] ", std::distance(data.begin(), data.end()));
    ImGui::SameLine();

    // Add/Remove buttons for dynamic containers
    ImGui::BeginDisabled(readOnly);
    if constexpr (CanAddValue<T>()) {
      ImGui::SameLine();
      if (ImGui::SmallButton("+")) {
        if (_e_createEditorCommand)
          Editor::History::CommandHistory::Create(
            Editor::History::MakeLambdaCommand(
              "Add Element to " + name,
              [&data] {
                AddValue(data, typename T::value_type{});
              }, [&data] {
                RemoveValue(data);
              }
            )
          );
        else
          AddValue(data, typename T::value_type{});
        changed = true;
      }
    }

    if constexpr (CanRemoveValue<T>()) {
      if constexpr (CanAddValue<T>())
        ImGui::SameLine();

      ImGui::BeginDisabled(std::distance(data.begin(), data.end()) == 0);
      if (ImGui::SmallButton("-")) {
        struct ValueHolder {
          mutable std::optional<typename T::value_type> value;
        };

        auto holder = std::make_shared<ValueHolder>();

        if (_e_createEditorCommand)
          Editor::History::CommandHistory::Create(
            Editor::History::MakeLambdaCommand(
              "Remove Element from " + name,
              [&data, holder] {
                holder->value = ExtractValue(data);
              },
              [&data, holder] {
                if (holder->value.has_value())
                  AddValue(data, std::move(*holder->value));
                else
                  AddValue(data, typename T::value_type{});
              }
            )
          );
        else
          ExtractValue(data);

        changed = true;
      }
      ImGui::EndDisabled();
    }
    ImGui::EndDisabled();

    if (opened) {
      // Render each element
      size_t i = 0;
      for (auto &element: data) {
        ImGui::PushID(static_cast<int>(i));

        // Recursively render the element
        if (const std::string Id = "Element " + std::to_string(i);
          _e_renderInEditorImpl(element, Id, readOnly, name + '[' + std::to_string(i) + ']'))
          changed = true;

        ImGui::PopID();
        ++i;
      }
    }

    ImGui::PopID();
    return changed;
  }

  template<IsMap T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
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

    ImGui::Text("[%zu] ", std::distance(data.begin(), data.end()));
    ImGui::SameLine();

    // Add/Remove buttons for dynamic containers
    ImGui::BeginDisabled(readOnly);
    if constexpr (requires {
      data.emplace(typename T::key_type{}, typename T::mapped_type{}); data.erase(data.begin());
    }) {
      ImGui::SameLine();
      if (ImGui::SmallButton("+")) {
        ImGui::OpenPopup("AddMapEntry");
      }

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
      ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5);
      if (ImGui::BeginPopup("AddMapEntry")) {
        static typename T::key_type newKey{};
        bool shouldAdd = false;

        if (ImGui::BeginTable((name + "Table").c_str(), 2, ImGuiTableFlags_SizingFixedFit)) {
          ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 75);
          ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 125);

          _e_createEditorCommand = false;
          _e_renderInEditorImpl(newKey, " New Key");
          _e_createEditorCommand = true;

          ImGui::EndTable();
        }

        ImGui::SetCursorPosX(35);
        if (ImGui::Button("Add", ImVec2(60, 0))) {
          if (data.find(newKey) == data.end()) {
            if (_e_createEditorCommand)
              Editor::History::CommandHistory::Create(
                Editor::History::MakeLambdaCommand(
                  "Add Element to " + name,
                  [&data, key = newKey] {
                    data[key] = typename T::mapped_type{};
                  }, [&data, key = newKey] {
                    data.erase(key);
                  }
                )
              );
            else
              data[newKey] = typename T::mapped_type{};

            changed = true;
            shouldAdd = true;
          }
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(135);
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
        if (_e_createEditorCommand) {
          struct ValueHolder {
            mutable std::optional<typename T::value_type> value;
          };

          auto holder = std::make_shared<ValueHolder>();

          Editor::History::CommandHistory::Create(
            Editor::History::MakeLambdaCommand(
              "Remove Element from " + name,
              [&data, holder] {
                if constexpr (requires { typename T::hasher; }) {
                  const auto node = data.extract(data.begin());
                  holder->value.emplace(std::move(node.key()), std::move(node.mapped()));
                } else {
                  const auto node = data.extract(std::prev(data.end()));
                  holder->value.emplace(std::move(node.key()), std::move(node.mapped()));
                }
              }, [&data, holder] {
                if (holder->value.has_value())
                  data.insert(std::move(*holder->value));
                else
                  data.insert(typename T::value_type{});
              }
            )
          );
        } else {
          if constexpr (requires { typename T::hasher; })
            data.erase(data.begin());
          else
            data.erase(std::prev(data.end()));
        }

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
          _e_renderInEditorImpl(k, "Key", true);
          ImGui::PopID();

          ImGui::PushID(("##" + name + "_value" + std::to_string(index)).c_str());
          if (_e_renderInEditorImpl(v, "Value", readOnly, "Value " + std::to_string(index) + " in " + name))
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

  template<IsPair T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = _e_renderTreeNode(name, ImGuiTreeNodeFlags_AllowItemOverlap);

    ImGui::TableNextColumn();
    ImGui::Text("[2]");

    ImGui::TableNextColumn();

    if (opened) {
      // Render each element
      ImGui::PushID(("##" + name + "_first").c_str());
      if (_e_renderInEditorImpl(data.first, "First", readOnly, "First in " + name))
        changed = true;
      ImGui::PopID();

      ImGui::PushID(("##" + name + "_second").c_str());
      if (_e_renderInEditorImpl(data.second, "Second", readOnly, "Second in " + name))
        changed = true;
      ImGui::PopID();
    }

    ImGui::PopID();
    return changed;
  }

  template<typename... Ts> static bool _e_renderInEditor(
    std::tuple<Ts...> &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = _e_renderTreeNode(name, ImGuiTreeNodeFlags_AllowItemOverlap);

    ImGui::TableNextColumn();
    ImGui::Text("[%zu]", sizeof...(Ts));
    ImGui::TableNextColumn();

    if (opened) {
      // Helper lambda to render each tuple element recursively
      auto renderElement = [&]<size_t Index>(std::integral_constant<size_t, Index>) {
        if constexpr (Index < sizeof...(Ts)) {
          ImGui::PushID(static_cast<int>(Index));

          const std::string elementId = "Element " + std::to_string(Index);
          if (auto &element = std::get<Index>(data);
            _e_renderInEditorImpl(element, elementId, readOnly, elementId + " in " + name)) {
            changed = true;
          }

          ImGui::PopID();
        }
      };

      // Render each tuple element using fold expression
      [&]<size_t... Indices>(std::index_sequence<Indices...>) {
        (renderElement(std::integral_constant<size_t, Indices>{}), ...);
      }(std::make_index_sequence<sizeof...(Ts)>{});
    }

    ImGui::PopID();
    return changed;
  }

  template<IsSmartPtr T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    return false;
  }

  template<IsReflectable T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    if constexpr (std::is_base_of_v<ICustomEditor, T>)
      return data.OnRenderInEditor(name, std::is_const_v<T>, readOnly);
    else
      return data._e_renderInEditor(readOnly);
  }

  template<IsReflectablePointer T> static bool _e_renderInEditor(
    T *data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    return false;
  }

  template<IsEnum T> static bool _e_renderInEditor(
    T &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    if (!ReflectionFactory::EnumRegistered<T>())
      return false;

    bool changed = false;

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", name.c_str());
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    static const auto &values = ReflectionFactory::GetEnumValues<T>();
    size_t currentIndex = 0;
    const std::string currentValue = ReflectionFactory::EnumValueToString(data);
    for (size_t i = 0; i < values.size(); ++i) {
      if (values[i] == currentValue) {
        currentIndex = i;
        break;
      }
    }

    ImGui::BeginDisabled(readOnly);
    if (ImGui::BeginCombo(
      ("##enum_" + name).c_str(), currentIndex < values.size() ? values[currentIndex].c_str() : "Unknown"
    )) {
      for (size_t i = 0; i < values.size(); ++i) {
        const bool isSelected = i == currentIndex;
        if (ImGui::Selectable(values[i].c_str(), isSelected) && i != currentIndex)
          if (const auto potentialVal = ReflectionFactory::StringToEnumValue<T>(values[i])) {
            if (_e_createEditorCommand)
              Editor::History::CommandHistory::Create(
                Editor::History::MakeLambdaCommand(
                  "Changed " + name + " in " + _e_currentEntityName,
                  [&data, newVal = *potentialVal] {
                    data = newVal;
                  },
                  [&data, old = data] {
                    data = old;
                  }
                )
              );
            else
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
  static bool _e_renderInEditor(
    glm::vec<N, T, Q> &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    using Type = std::remove_cvref_t<T>;

    ImGui::BeginDisabled(readOnly);
    bool changed = false;

    if constexpr (std::is_floating_point_v<Type> || (std::is_integral_v<Type> && !std::is_same_v<T, bool>))
      changed = DragWithHistory(data, name);
    else {
      static constexpr std::array labels = {"X:", " Y:", " Z:", " W:"};
      for (glm::length_t i = 0; i < N; ++i) {
        if (i > 0)
          ImGui::SameLine();

        ImGui::Text("%s", labels[i]);
        ImGui::SameLine();

        _e_renderInEditor(data[i], name, readOnly, name + '[' + std::to_string(i) + ']');
      }
    }

    ImGui::EndDisabled();
    return changed;
  }

  template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q> requires (
    2 <= C && C <= 4 && 2 <= R && R <= 4
  )
  static bool _e_renderInEditor(
    glm::mat<C, R, T, Q> &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    bool changed = false;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    const bool opened = _e_renderTreeNode(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::Text("[%dx%d]", C, R);

    ImGui::BeginDisabled(readOnly);
    if (opened) {
      for (glm::length_t col = 0; col < C; ++col) {
        ImGui::PushID(("##" + name + "_row" + std::to_string(col)).c_str());

        const std::string elementLabel = "Row " + std::to_string(col);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", elementLabel.c_str());

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        changed = DragWithHistory(data[col], elementLabel + " of " + name);

        ImGui::PopID();
      }
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
    std::variant<Ts...> &data, const std::string &name, const bool readOnly, const std::string & = ""
  ) {
    static constexpr std::array<std::string_view, sizeof...(Ts)> arr = _e_getVariantTypes<Ts...>();

    bool changed = false;

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);

    ImGui::PushID(("##" + name).c_str());
    ImGui::AlignTextToFramePadding();
    const bool opened = ImGui::TreeNodeEx(
      name.c_str(),
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_SpanAllColumns
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
                  if (_e_createEditorCommand)
                    Editor::History::CommandHistory::Create(
                      Editor::History::MakeLambdaCommand(
                        "Changed " + name + " in " + _e_currentEntityName, [&data] {
                          data = TargetType{};
                        }, [&data, old = data] {
                          data = old;
                        }
                      )
                    );
                  else
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
          if (_e_renderInEditorImpl(value, "Value", readOnly, "Value in " + name)) {
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

  template<typename T> static bool _e_renderInEditorImpl(
    T &data, const std::string &name, const bool readOnly, const std::string &id
  ) {
    if constexpr (HasRenderInEditorFunction<T>) {
      if constexpr ((IsReflectable<T> && !std::is_base_of_v<ICustomEditor, T>) || !IsReflectable<T>) {
        if constexpr (!IsEnum<T>)
          ImGui::TableNextRow();
        if constexpr (IsNonRecursive<T>) {
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text("%s", name.c_str());
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1);
        }
      }

      if (_e_renderInEditor(data, name, readOnly, id))
        return true;
    }
    return false;
  }
}

#endif

#endif //RENDER_IN_EDITOR_HPP
