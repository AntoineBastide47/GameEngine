//
// Serializable.hpp
// Author: Antoine Bastide
// Date: 19.06.2025
//

#ifndef REFLECTABLE_HPP
#define REFLECTABLE_HPP

#include <string_view>

#include "Engine/Macros/Utils.hpp"
#include "Engine/Reflection/Load.hpp"
#include "Engine/Reflection/RenderInEditor.hpp"
#include "Engine/Reflection/Save.hpp"

namespace Editor {
  class EntityInspector;
}

namespace Engine::Reflection {
  class ReflectionFactory;
}

#define _e_SERIALIZE_RECORD \
  friend class Editor::EntityInspector; \
  friend class Engine::Reflection::ReflectionFactory; \
  template<typename T> friend bool _e_renderInEditor(T &, const std::string &, bool, const std::string &); \
  public: \
    [[nodiscard]] std::string_view ClassNameQualified() const override { return ENGINE_CLASS_NAME_FULLY_QUALIFIED; } \
    [[nodiscard]] std::string_view ClassName() const override { return ENGINE_CLASS_NAME; }
#define _e_SERIALIZE_STRING "serialize"
#define _e_NON_SERIALIZABLE_STRING "non_serializable"
#define _e_SHOW_IN_INSPECTOR_STRING "show_in_inspector"
#define _e_HIDE_IN_INSPECTOR_STRING "hide_in_inspector"

// Since only the tool itself uses the annotations, only make the macros add them when the tool is run
// This should prevent compilation errors on compilers that do not support custom annotation (ex: MSVC)
// This allows the engine to be compiled with clang, gcc or MSVC but enforces the Header Forge to be compiled with clang
#ifdef HEADER_FORGE_ENABLE_ANNOTATIONS
#define ENGINE_SHOW_IN_INSPECTOR __attribute__((annotate(_e_SHOW_IN_INSPECTOR_STRING)))
#define ENGINE_HIDE_IN_INSPECTOR __attribute__((annotate(_e_HIDE_IN_INSPECTOR_STRING)))
#define ENGINE_SERIALIZE __attribute__((annotate(_e_SERIALIZE_STRING))) ENGINE_SHOW_IN_INSPECTOR
#define ENGINE_NON_SERIALIZABLE __attribute__((annotate(_e_NON_SERIALIZABLE_STRING)))
#else
#define ENGINE_SERIALIZE
#define ENGINE_NON_SERIALIZABLE
#define ENGINE_SHOW_IN_INSPECTOR
#define ENGINE_HIDE_IN_INSPECTOR
#endif

#define ENGINE_SERIALIZE_HIDDEN ENGINE_SERIALIZE ENGINE_HIDE_IN_INSPECTOR
#define ENGINE_NON_SERIALIZABLE_SHOWN ENGINE_NON_SERIALIZABLE ENGINE_SHOW_IN_INSPECTOR

namespace Engine::Reflection {
  /// Class used to accurately save classes
  struct Reflectable {
    virtual ~Reflectable() = default;
    /// @returns The name of the current class with its namespace and parent classes as a string
    [[nodiscard]] virtual std::string_view ClassNameQualified() const = 0;
    /// @returns The name of the current class without its namespace and parent classes as a string
    [[nodiscard]] virtual std::string_view ClassName() const = 0;

    /// Serializes the current class instance
    virtual void _e_save(Format format, Engine::JSON &json) const = 0;
    /// Called when this class instance is serialized/saved
    /// @param format The format in which to save the data
    /// @param json The json representation of this class instance
    virtual void OnSerialize(Format format, Engine::JSON &json) const {}

    /// Loads a class instance
    virtual void _e_load(Format format, const Engine::JSON &json) = 0;
    /// Called when this class instance is deserialized/loaded
    /// @param format The format in which to load the data
    /// @param json The json representation of this class instance
    virtual void OnDeserialize(Format format, const Engine::JSON &json) {}

    #if ENGINE_EDITOR
    bool _e_renderInEditorImpl(bool readOnly, const std::string &name);

    virtual bool _e_renderInEditor(const bool readOnly) {
      return false;
    }

    /// Called when an interaction with script variables occurs in the editor
    virtual void OnEditorValueChanged() {}
    #endif
  };
}

#endif //REFLECTABLE_HPP
