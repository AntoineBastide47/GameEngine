//
// Serializer.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <fstream>
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
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static Engine::JSON ToJson(const T &data) {
        Engine::JSON json;
        _e_saveImpl(data, JSON, json);
        return json;
      }

      /**
       * Converts the given data to a JSON representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @param prettyPrint If the JSON string needs to be pretty printed, false by default
       * @param indentChar The character used to indent the json string when pretty printed, set to whitespace by default
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToJsonString(
        const T &data, const bool prettyPrint = false, const char indentChar = ' '
      ) {
        return ToJson(data).Dump(prettyPrint - 1, indentChar);
      }

      /**
       * Converts the given data to a JSON representation and saves it to the given file
       * @tparam T The type of the data
       * @param data The data to convert
       * @param filePath The file in which the converted data will be stored
       * @param prettyPrint If the JSON string needs to be pretty printed, false by default
       * @param indentChar The character used to indent the json string when pretty printed, set to whitespace by default
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static void ToJsonToFile(
        const T &data, const std::string &filePath, const bool prettyPrint = false, const char indentChar = ' '
      ) {
        std::ofstream file(filePath);
        file << ToJson(data).Dump(prettyPrint - 1, indentChar);
      }
  };
}

#endif //SERIALIZER_HPP
