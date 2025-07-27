//
// ReflectionFactory.hpp
// Author: Antoine Bastide
// Date: 17.07.2025
//

#ifndef REFLECTION_FACTORY_HPP
#define REFLECTION_FACTORY_HPP

namespace Engine::Reflection {
  class ReflectionFactory final {
    inline static std::unordered_map<std::string, std::string> engineNameToTypeIdName;
    inline static std::unordered_map<std::string, std::function<void*()>> typeIdNameToFactory;
    public:
      template<typename T> static void RegisterType(const std::string &type) {
        engineNameToTypeIdName[type] = typeid(T).name();
        typeIdNameToFactory[typeid(T).name()] = [] {
          return static_cast<void *>(new T());
        };
      }

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

      template<typename T> static std::shared_ptr<T> CreateUnique(const std::string &type) {
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
  };
}

#endif //REFLECTION_FACTORY_HPP
