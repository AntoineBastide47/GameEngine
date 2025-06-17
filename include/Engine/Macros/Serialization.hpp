//
// Serialization.hpp
// Author: Antoine Bastide
// Date: 13/06/2025
//

#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

namespace Engine::Serialization {
  class Serializer final {};
  class Deserializer final {};
}

#define SERIALIZE_FIELD_STRING "serialize_field"
#define NON_SERIALIZABLE_FIELD "non_serializable_field"
#define SERIALIZE_POINTER_STRING "serialize_pointer"

#define ENGINE_SERIALIZE_RECORD \
  friend class Engine::Serialization::Serializer; \
  friend class Engine::Serialization::Deserializer;
#define ENGINE_SERIALIZE_FIELD __attribute__((annotate(SERIALIZE_FIELD_STRING)))
#define ENGINE_NON_SERIALIZABLE_FIELD __attribute__((annotate(NON_SERIALIZABLE_FIELD)))
#define ENGINE_SERIALIZE_POINTER __attribute__((annotate(SERIALIZE_POINTER_STRING)))

#endif //SERIALIZATION_HPP
