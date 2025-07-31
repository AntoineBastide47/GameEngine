//
// Load.hpp
// Author: Antoine Bastide
// Date: 16.07.2025
//

#ifndef LOAD_HPP
#define LOAD_HPP

#include <glm/glm.hpp>

#include "Engine/Data/JSON.hpp"
#include "Engine/Reflection/Concepts.hpp"
#include "Engine/Reflection/ReflectionFactory.hpp"

namespace Engine2D {
  class Component2D;
}

namespace Engine::Reflection {
  template<typename T> static void _e_loadImpl(T &result, const Format format, const Engine::JSON &json) {
    if constexpr (HasLoadFunction<T, Format>)
      _e_load(result, format, json);
    else
      static_assert(
        _e_f<T>, R"(
No save overloads were found for the requested type.
 - if the type is a part of the STL, convert it to a supported STL type before the load call
 - if the type is not a part of the STL, make sure the type publicly inherits from Engine::Reflection::Reflectable
)"
      );
  }

  template<IsNumber T> static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON)
      result = static_cast<T>(json.GetNumber());
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, std::string>
  static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON)
      result = json.GetString();
  }

  template<typename T> requires std::is_same_v<std::remove_cvref_t<T>, bool>
  static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON)
      result = json.GetBool();
  }

  template<typename T> requires std::is_enum_v<T>
  static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      auto &value = json.GetNumber();
      if (std::trunc(value) != value)
        throw std::runtime_error("Non-integral value for enum");

      result = static_cast<T>(static_cast<std::underlying_type_t<T>>(value));
    }
  }

  template<IsContainer T> static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      const auto &array = json.GetArray();

      if constexpr (requires { result.clear(); })
        result.clear();

      if constexpr (requires { result.reserve(array.size()); })
        result.reserve(array.size());

      if constexpr (requires { result.insert(result.end(), typename T::value_type{}); }) {
        for (const auto &e: array) {
          typename T::value_type value;
          _e_loadImpl(value, format, e);
          result.insert(result.end(), std::move(value));
        }
      } else if constexpr (requires { result.insert(typename T::value_type{}); }) {
        for (const auto &e: array) {
          typename T::value_type value;
          _e_loadImpl(value, format, e);
          result.insert(std::move(value));
        }
      } else if constexpr (requires { result.insert_after(result.before_begin(), typename T::value_type{}); }) {
        auto it = result.before_begin();
        for (const auto &e: array) {
          typename T::value_type value;
          _e_loadImpl(value, format, e);
          it = result.insert_after(it, std::move(value));
        }
      } else if constexpr (requires { result.push_back(typename T::value_type{}); }) {
        for (const auto &e: array) {
          typename T::value_type value;
          _e_loadImpl(value, format, e);
          result.push_back(std::move(value));
        }
      } else if constexpr (requires { result.emplace_back(typename T::value_type{}); }) {
        for (const auto &e: array) {
          typename T::value_type value;
          _e_loadImpl(value, format, e);
          result.emplace_back(std::move(value));
        }
      } else if constexpr (std::is_same_v<std::decay_t<T>, std::array<typename T::value_type, T().size()>>) {
        if (array.size() != T().size()) {
          throw std::runtime_error("JSON array size mismatch for std::array");
        }

        if constexpr (requires { result.resize(array.size()); })
          result.resize(array.size());

        std::size_t i = 0;
        for (const auto &e: array)
          _e_loadImpl(result[i++], format, e);
      } else {
        static_assert(false, "Container type not supported for recursive insertion");
      }
    }
  }

  template<IsMap T> static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      const auto &obj = json.GetObject();

      if constexpr (requires { result.clear(); })
        result.clear();
      if constexpr (requires { result.resize(obj.size()); })
        result.reserve(obj.size());

      for (const auto &[k, v]: obj) {
        typename T::key_type key;
        typename T::mapped_type value;

        if constexpr (std::is_same_v<typename T::key_type, std::string>)
          key = k;
        else
          _e_loadImpl(key, format, k);

        _e_loadImpl(value, format, v);
        result.emplace(std::move(key), std::move(value));
      }
    }
  }

  template<IsPair T> static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      const auto &arr = json.GetArray();
      _e_loadImpl(result.first, format, arr.at(0));
      _e_loadImpl(result.second, format, arr.at(1));
    }
  }

  template<IsSmartPtr T> static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      if (json.IsNull()) {
        result = nullptr;
        return;
      }

      if constexpr (IsSTL<typename T::element_type>) {
        if constexpr (IsSharedPtr<T>)
          result = std::make_shared<typename T::element_type>();
        else
          result = std::make_unique<typename T::element_type>();

        _e_loadImpl(*result, format, json);
      } else {
        if constexpr (IsSharedPtr<T>)
          result = ReflectionFactory::CreateShared<typename T::element_type>(json["type"].GetString());
        else
          result = ReflectionFactory::CreateUnique<typename T::element_type>(json["type"].GetString());

        result->_e_load(format, json["data"]);

        if constexpr (!std::is_base_of_v<Engine2D::Component2D, typename T::element_type>)
          result->OnDeserialize(format, json["data"]);
      }
    }
  }

  template<IsNotSTL T> static void _e_load(T &result, const Format format, const Engine::JSON &json) {
    if constexpr (requires { result._e_load(format, json); }) {
      result._e_load(format, json);
      result.OnDeserialize(format, json);
    } else
      static_assert(_e_f<T>, "Missing load function. Ensure the type uses it's SERIALIZE_* macro.");
  }

  template<typename... Ts> static void _e_load(std::tuple<Ts...> &t, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      std::size_t i = 0;
      std::apply(
        [&](Ts &... elems) {
          (([&] {
            _e_loadImpl(elems, format, json.At(i++));
          }()), ...);
        }, t
      );
    }
  }

  template<glm::length_t N, typename T, glm::qualifier Q>
  static void _e_load(glm::vec<N, T, Q> &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      if (!json.IsArray() || json.Size() != N)
        throw std::runtime_error("Invalid JSON array size for glm::vec");

      for (glm::length_t i = 0; i < N; ++i)
        _e_loadImpl(result[i], format, json[i]);
    }
  }

  template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
  static void _e_load(glm::mat<C, R, T, Q> &result, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      if (!json.IsArray() || json.Size() != C)
        throw std::runtime_error("Invalid JSON array size for glm::mat columns");

      for (glm::length_t col = 0; col < C; ++col) {
        const auto &colJson = json[col];
        if (!colJson.IsArray() || colJson.Size() != R)
          throw std::runtime_error("Invalid JSON array size for glm::mat column elements");

        for (glm::length_t row = 0; row < R; ++row)
          _e_loadImpl(result[col][row], format, colJson[row]);
      }
    }
  }

  template<typename... Ts> static void _e_load(std::variant<Ts...> &v, const Format format, const Engine::JSON &json) {
    if (format == JSON) {
      if (!json.IsObject() || !json.Contains("index") || !json.Contains("value"))
        throw std::runtime_error("Invalid JSON for variant");

      const size_t idx = static_cast<size_t>(json["index"].GetNumber());
      const auto &valueJson = json["value"];

      auto loader = [&]<typename T0>([[maybe_unused]] T0 typeTag) {
        using T = typename T0::type;
        T temp;
        _e_loadImpl(temp, format, valueJson);
        v = std::move(temp);
      };

      ((idx == std::variant<Ts...>::template index_of<Ts>() ? (loader(std::type_identity<Ts>{}), true) : false) || ...);
    }
  }
}

#endif //LOAD_HPP
