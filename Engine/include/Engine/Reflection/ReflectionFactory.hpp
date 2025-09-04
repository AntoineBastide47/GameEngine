//
// ReflectionFactory.hpp
// Author: Antoine Bastide
// Date: 17.07.2025
//

#ifndef REFLECTION_FACTORY_HPP
#define REFLECTION_FACTORY_HPP

#include <functional>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <unordered_map>

namespace Engine2D {
  class Game2D;
}

namespace Engine::Reflection {
  class ReflectionFactory final {
    friend class Engine2D::Game2D;
    public:
      /// Registers a type so that it can be instantiated using a string instead of the type declaration
      /// @tparam T The type to register
      /// @param type The string version of the type to register
      template<typename T> static void RegisterType(const std::string &type) {
        engineNameToTypeIdName[type] = typeid(T).name();
        typeIdNameToFactory[typeid(T).name()] = [] {
          return static_cast<void *>(new T());
        };
      }

      /// @returns A shared pointer of the given registered type string representation cast to a T type
      template<typename T> static std::shared_ptr<T> CreateShared(const std::string &type) {
        if (!engineNameToTypeIdName.contains(type))
          throw std::runtime_error("Unknown type name: " + type);

        if (!typeIdNameToFactory.contains(engineNameToTypeIdName.at(type)))
          throw std::runtime_error("No creator for type: " + type);

        const auto basePtr = typeIdNameToFactory.at(engineNameToTypeIdName.at(type))();
        auto typedPtr = static_cast<T *>(basePtr);
        if (!typedPtr)
          throw std::runtime_error("ReflectionFactory: Type mismatch for '" + type + "'");
        return std::shared_ptr<T>(typedPtr);
      }

      /// @returns A unique pointer of the given registered type string representation cast to a T type
      template<typename T> static std::unique_ptr<T> CreateUnique(const std::string &type) {
        if (!engineNameToTypeIdName.contains(type))
          throw std::runtime_error("Unknown type name: " + type);

        if (!typeIdNameToFactory.contains(engineNameToTypeIdName.at(type)))
          throw std::runtime_error("No creator for type: " + type);

        const auto basePtr = typeIdNameToFactory.at(engineNameToTypeIdName.at(type))();
        auto typedPtr = static_cast<T *>(basePtr);
        if (!typedPtr)
          throw std::runtime_error("ReflectionFactory: Type mismatch for '" + type + "'");
        return std::unique_ptr<T>(typedPtr);
      }

      /// Registers an enum for reflection
      /// @tparam T The enum to register
      /// @param enumName The name of the enum to register
      /// @param values The values of the enum to register
      template<IsEnum T> static void RegisterEnum(
        const std::string &enumName, std::initializer_list<std::pair<std::string, T>> values
      ) {
        enumTypeIdToName[typeid(T).name()] = enumName;

        // Create bidirectional mappings for this enum type
        auto &stringToValue = enumStringToValue[typeid(T).name()];
        auto &valueToString = enumValueToString[typeid(T).name()];

        for (const auto &[name, value]: values) {
          // Store as underlying type to avoid template complexity
          auto underlyingValue = static_cast<std::underlying_type_t<T>>(value);

          stringToValue[name] = underlyingValue;
          valueToString[underlyingValue] = name;
        }

        // Store all value names for iteration
        std::vector<std::string> valueNames;
        valueNames.reserve(values.size());
        for (const auto &[name, value]: values)
          valueNames.push_back(name);

        enumValues[typeid(T).name()] = std::move(valueNames);
      }

      /// @returns True if the given enum has been registered, False if not
      template<IsEnum T> static bool EnumRegistered() {
        return enumTypeIdToName.contains(typeid(T).name());
      }


      /// @returns The name of the given if it has been registered, "" if not
      template<IsEnum T> static std::string EnumName() {
        if (const auto typeId = typeid(T).name(); enumTypeIdToName.contains(typeId))
          return enumTypeIdToName.at(typeId);
        return "";
      }

      /// @returns The string version of the given value, "" if it hasn't been registered
      template<IsEnum T> static std::string EnumValueToString(T value) {
        const auto typeId = typeid(T).name();
        const auto it = enumValueToString.find(typeId);
        if (it == enumValueToString.end())
          return "";

        auto underlyingValue = static_cast<std::underlying_type_t<T>>(value);
        auto valueIt = it->second.find(underlyingValue);
        if (valueIt == it->second.end())
          return "";

        return valueIt->second;
      }

      /// @returns The value of the given string value, std::nullopt if it hasn't been registered
      template<IsEnum T> static std::optional<T> StringToEnumValue(const std::string &valueName) {
        if (!EnumRegistered<T>())
          return std::nullopt;

        const auto typeId = typeid(T).name();
        const auto it = enumStringToValue.find(typeId);
        if (it == enumStringToValue.end())
          return std::nullopt;

        const auto valueIt = it->second.find(valueName);
        if (valueIt == it->second.end())
          return std::nullopt;

        return static_cast<T>(valueIt->second);
      }

      /// @returns All the values associated to the given enum
      template<IsEnum T> static std::vector<std::string> GetEnumValues() {
        if (const auto it = enumValues.find(typeid(T).name()); it != enumValues.end())
          return it->second;
        return {};
      }
    private:
      inline static std::unordered_map<std::string, std::string> engineNameToTypeIdName;
      inline static std::unordered_map<std::string, std::function<void*()>> typeIdNameToFactory;

      inline static std::unordered_map<std::string, std::string> enumTypeIdToName;
      inline static std::unordered_map<std::string, std::vector<std::string>> enumValues;

      inline static std::unordered_map<std::string, std::unordered_map<std::string, int64_t>> enumStringToValue;
      inline static std::unordered_map<std::string, std::unordered_map<int64_t, std::string>> enumValueToString;

      static void cleanup() {
        typeIdNameToFactory.clear();
        engineNameToTypeIdName.clear();
        enumTypeIdToName.clear();
        enumValues.clear();
        enumStringToValue.clear();
        enumValueToString.clear();
      }
  };
}

#endif //REFLECTION_FACTORY_HPP
