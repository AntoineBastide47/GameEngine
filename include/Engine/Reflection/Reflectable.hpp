//
// Serializable.hpp
// Author: Antoine Bastide
// Date: 19.06.2025
//

#ifndef REFLECTABLE_HPP
#define REFLECTABLE_HPP

#include <string_view>

#include "Engine/Reflection/Save.hpp"
#include "Engine/Reflection/Load.hpp"

// Since only the tool itself uses the annotations, only make the macros add them when the tool is run
// This should prevent compilation errors on compilers that do not support custom annotation (ex: MSVC)
// This allows the engine to be compiled with clang, gcc or MSVC but enforces the Header Forge to be compiled with clang
#ifdef HEADER_FORGE_ENABLE_ANNOTATIONS
  #define ENGINE_SERIALIZE __attribute__((annotate(_e_SERIALIZE_STRING)))
  #define ENGINE_NON_SERIALIZABLE __attribute__((annotate(_e_NON_SERIALIZABLE_STRING)))
#else
#define ENGINE_SERIALIZE
#define ENGINE_NON_SERIALIZABLE
#endif

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
    /// Loads a class instance
    virtual void _e_load(Format format, const Engine::JSON &json) = 0;
    /// Called when this class instance is serialized/saved
    /// @param format The format in which to save the data
    /// @param json The json representation of this class instance
    virtual void OnSerialize(Format format, Engine::JSON &json) const {}
    /// Called when this class instance is deserialized/loaded
    /// @param format The format in which to load the data
    /// @param json The json representation of this class instance
    virtual void OnDeserialize(Format format, const Engine::JSON &json) {}
  };
}

#endif //REFLECTABLE_HPP
