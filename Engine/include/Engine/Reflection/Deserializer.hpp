//
// Deserializer.hpp
// Author: Antoine Bastide
// Date: 16.07.2025
//

#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP

#include <fstream>
#include <string_view>

#include "Engine/Reflection/Concepts.hpp"
//#include "Engine/Reflection/Load.hpp"
#include "Engine/Data/JSON.hpp"
#include "Engine/Data/JsonParser.hpp"

namespace Engine::Reflection {
  template<typename T> static void _e_loadImpl(T &, Format, const Engine::JSON &);

  class Deserializer final {
    public:
      /**
       * Converts the given data to a JSON representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static T FromJson(const Engine::JSON &data) {
        T result;
        _e_loadImpl(result, JSON, data);
        return result;
      }

      /**
       * Converts the given data to a JSON representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static T FromJsonString(const std::string_view &data) {
        JSONParser parser(data);
        T result;
        _e_loadImpl(result, JSON, parser.Parse());
        return result;
      }

      /**
       * Converts the given file to a JSON representation
       * @tparam T The type of the data
       * @param filePath The file in which the data to convert in located
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static T FromJsonFromFile(const std::string &filePath) {
        std::ifstream file(filePath);
        JSONParser parser(file);
        T result;
        _e_loadImpl(result, JSON, parser.Parse());
        return result;
      }
  };
}

#endif //DESERIALIZER_HPP
