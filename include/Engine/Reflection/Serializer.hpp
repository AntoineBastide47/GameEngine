//
// Serializer.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <sstream>
#include <string>

#include "Concepts.hpp"
#include "Save.hpp"

namespace Engine::Reflection {
  class Serializer final {
    public:
      /**
       * Converts the given data to a JSON representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @param prettyPrint If the JSON string needs to be pretty printed, false by default
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToJson(const T &data, const bool prettyPrint = false) {
        std::ostringstream os;
        _e_saveImpl(data, JSON, prettyPrint, 0, &os);
        return os.str();
      }

      /**
       * Converts the given data to a text representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToText(const T &data) {
        std::ostringstream os;
        _e_saveImpl(data, TEXT, false, 0, &os);
        return os.str();
      }

      /**
       * Converts the given data to a binary representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToBinary(const T &data) {
        std::ostringstream os;
        _e_saveImpl(data, BINARY, false, 0, &os);
        return os.str();
      }
  };
}

#endif //SERIALIZER_HPP
