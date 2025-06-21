//
// Serialization.hpp
// Author: Antoine Bastide
// Date: 13/06/2025
//

#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include "Utils.hpp"

namespace Engine::Serialization {
  class Serializer;
  class Deserializer;
}

constexpr size_t hash(const char *str) {
  size_t h = 14695981039346656037ull;
  while (*str) {
    h ^= static_cast<unsigned char>(*str++);
    h *= 1099511628211ull;
  }
  return h;
}

#define _e_SERIALIZE_RECORD \
  friend class Engine::Serialization::Serializer; \
  friend class Engine::Serialization::Deserializer; \
  public: \
    static constexpr size_t StaticTypeId() { return hash(ENGINE_CLASS_NAME.data()); } \
    [[nodiscard]] inline size_t TypeId() const { return StaticTypeId(); } \
    [[nodiscard]] inline std::string_view GetClass() const { return ENGINE_CLASS_NAME; }

#define ENGINE_SERIALIZE_CLASS \
  _e_SERIALIZE_RECORD \
  private:

#define ENGINE_SERIALIZE_STRUCT _e_SERIALIZE_RECORD

#define _e_SERIALIZE_STRING "serialize"
#define _e_NON_SERIALIZABLE "non_serializable"

#define ENGINE_SERIALIZE __attribute__((annotate(_e_SERIALIZE_STRING)))
#define ENGINE_NON_SERIALIZABLE __attribute__((annotate(_e_NON_SERIALIZABLE)))

#endif //SERIALIZATION_HPP
