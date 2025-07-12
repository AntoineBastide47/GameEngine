//
// Serializable.hpp
// Author: Antoine Bastide
// Date: 19.06.2025
//

#ifndef REFLECTABLE_HPP
#define REFLECTABLE_HPP

#include <string_view>

#include "Concepts.hpp"
#include "Save.hpp"

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
  class Reflectable {
    public:
      virtual ~Reflectable() = default;
      /// @returns The name of the current class with its namespace and parent classes as a string
      [[nodiscard]] virtual std::string_view ClassNameQualified() const = 0;
      /// @returns The name of the current class without its namespace and parent classes as a string
      [[nodiscard]] virtual std::string_view ClassName() const = 0;
      /// @returns The serialized version of this variable in the given format
      virtual void _e_save(Format format, std::ostringstream &os, bool prettyPrint, int indent) const = 0;
  };
}

#endif //REFLECTABLE_HPP
