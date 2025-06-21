//
// Serializable.hpp
// Author: Antoine Bastide
// Date: 19.06.2025
//

#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <string_view>

namespace Engine::Serialization {
  /// Class used to accurately save pointers to polymorphic classes
  class Serializable {
    public:
      virtual ~Serializable() = default;
      /// @returns The name of the current class as a string
      [[nodiscard]] virtual inline std::string_view GetClass() const = 0;
      /// @returns A hash of the name of the current class
      [[nodiscard]] virtual inline size_t TypeId() const = 0;
  };
}

#endif //SERIALIZABLE_HPP
