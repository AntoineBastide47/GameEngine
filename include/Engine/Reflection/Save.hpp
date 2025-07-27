//
// Save.hpp
// Author: Antoine Bastide
// Date: 22.06.2025
//

#ifndef SAVE_HPP
#define SAVE_HPP

#include <glm/glm.hpp>

#include "Engine/Reflection/Concepts.hpp"
#include "Engine/Macros/Utils.hpp"
#include "Engine/Data/JSON.hpp"

namespace Engine::Reflection {
  class ReflectionFactory;
}

#define _e_SERIALIZE_RECORD \
  friend class Engine::Reflection::ReflectionFactory; \
  public: \
    [[nodiscard]] std::string_view ClassNameQualified() const override { return ENGINE_CLASS_NAME_FULLY_QUALIFIED; } \
    [[nodiscard]] std::string_view ClassName() const override { return ENGINE_CLASS_NAME; }
#define _e_SERIALIZE_STRING "serialize"
#define _e_NON_SERIALIZABLE_STRING "non_serializable"

namespace Engine::Reflection {
  template<typename T> static void _e_saveImpl(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if constexpr (HasSaveFunction<T, Format>)
      _e_save(data, format, json);
    else
      static_assert(
        _e_f<T>, R"(
No save overloads were found for the requested type.
 - if the type is a part of the STL, convert it to a supported STL type before the save call
 - if the type is not a part of the STL, make sure the type publicly inherits from Engine::Reflection::Reflectable
)"
      );
  }

  template<IsNumber T> static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (format == JSON)
      json = data;
  }

  template<IsString T> static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (format == JSON)
      json = data;
  }

  template<typename T> requires std::is_same_v<std::decay_t<T>, bool>
  static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (format == JSON)
      json = data;
  }

  template<IsContainer T> static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (format == JSON) {
      json = JSON::Array();
      for (const auto &item: data) {
        Engine::JSON value;
        _e_saveImpl(item, format, value);
        json.PushBack(value);
      }
    }
  }

  template<IsMap T> static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (format == JSON) {
      json = JSON::Object();
      for (const auto &[k, v]: data) {
        Engine::JSON key;
        _e_saveImpl(k, format, key);
        _e_saveImpl(v, format, json[key.Dump()]);
      }
    }
  }

  template<typename T> requires IsPair<std::remove_cvref_t<T>>::value
  static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (format == JSON) {
      json = JSON::Array({{}, {}});
      _e_saveImpl(data.first, format, json[0]);
      _e_saveImpl(data.second, format, json[1]);
    }
  }

  template<typename... Ts>
  static void _e_save(
    const std::tuple<Ts...> &t, const Format format, Engine::JSON &json
  ) {
    if (format == JSON) {
      json = JSON::Array();
      std::apply(
        [&](const auto &... elems) {
          (([&] {
            Engine::JSON value;
            _e_saveImpl(elems, format, value);
            json.PushBack(value);
          }()), ...);
        }, t
      );
    }
  }

  template<IsTuple T> static void _e_save(
    const T &t, const Format format, Engine::JSON &json
  ) {
    if (format == JSON) {
      json = JSON::Array();
      std::apply(
        [&](const auto &... elems) {
          Engine::JSON value;
          _e_saveImpl(std::make_tuple(elems...), format, value);
          json.PushBack(value);
        }, t
      );
    }
  }

  template<IsSmartPtr T> static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if (data) {
      if constexpr (IsSTL<typename T::element_type>) {
        _e_saveImpl(*data, format, json);
      } else {
        json = JSON::Object();
        _e_saveImpl(data->ClassNameQualified(), format, json["_e_ptr_type"]);
        data->_e_save(format, json["_e_ptr_data"]);
      }
    } else
      json = {};
  }

  template<IsNotSTL T> static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    if constexpr (requires {
      data._e_save(format, json);
    })
      data._e_save(format, json);
    else
      static_assert(
        _e_f<T>,
        "Missing save function. Ensure the type uses it's SERIALIZE_* macro."
      );
  }

  template<typename T> requires std::is_enum_v<T>
  static void _e_save(
    const T &data, const Format format, Engine::JSON &json
  ) {
    _e_save(static_cast<std::underlying_type_t<T>>(data), format, json);
  }

  template<glm::length_t N, typename T, glm::qualifier Q> static void _e_save(
    const glm::vec<N, T, Q> &data, const Format format, Engine::JSON &json
  ) {
    if constexpr (!HasSaveFunction<T, Format>)
      static_assert(false, "The content of this glm::vec is not serializable");

    if (format == JSON) {
      json = JSON::Array();
      for (glm::length_t i = 0; i < data.length(); ++i) {
        Engine::JSON value;
        _e_save(data[i], format, value);
        json.PushBack(value);
      }
    }
  }

  template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
  static void _e_save(
    const glm::mat<C, R, T, Q> &data, const Format format, Engine::JSON &json
  ) {
    if constexpr (!HasSaveFunction<T, Format>)
      static_assert(false, "The content of this glm::mat is not serializable");

    if (format == JSON) {
      json = JSON::Array();
      for (glm::length_t col = 0; col < C; ++col) {
        Engine::JSON colJson = JSON::Array();
        for (glm::length_t row = 0; row < R; ++row) {
          Engine::JSON value;
          _e_save(data[col][row], format, value);
          colJson.PushBack(value);
        }
        json.PushBack(colJson);
      }
    }
  }

  template<typename... Ts> static void _e_save(
    const std::variant<Ts...> &v, const Format format, Engine::JSON &json
  ) {
    if (format == JSON) {
      json = JSON::Object();
      json["index"] = v.index();

      Engine::JSON value;
      std::visit(
        [&](const auto &elem) {
          _e_save(elem, format, value);
        }, v
      );
      json["value"] = value;
    }
  }
}

#endif //SAVE_HPP
