//
// Save.hpp
// Author: Antoine Bastide
// Date: 22.06.2025
//

#ifndef SAVE_HPP
#define SAVE_HPP

#include <sstream>

#include "Concepts.hpp"
#include "../Macros/Utils.hpp"

#define _e_SERIALIZE_RECORD \
  public: \
    [[nodiscard]] inline std::string_view ClassNameQualified() const { return ENGINE_CLASS_NAME_FULLY_QUALIFIED; } \
    [[nodiscard]] inline std::string_view ClassName() const { return ENGINE_CLASS_NAME; } \

#define _e_SERIALIZE_STRING "serialize"
#define _e_NON_SERIALIZABLE_STRING "non_serializable"

namespace Engine::Reflection {
  template<typename> static constexpr bool _e_f = false;

  template<typename T> static void _e_saveImpl(
    const T &data, Format format, const bool prettyPrint, const int indent = 0, std::ostringstream *_os = nullptr
  ) {
    std::ostringstream local;
    std::ostringstream &os = _os ? *_os : local;

    if constexpr (hasSaveFunction<T>::value)
      _e_save(data, format, os, prettyPrint, indent);
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
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if (format == JSON) {
      applyIndent(os, prettyPrint, indent);
      os << data;
    }
  }

  template<IsString T> static void _e_save(
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if (format == JSON) {
      applyIndent(os, prettyPrint, indent);
      os.put('"');
      os << data;
      os.put('"');
    }
  }

  template<typename T> requires std::is_same_v<std::decay_t<T>, bool>
  static void _e_save(
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if (format == JSON) {
      applyIndent(os, prettyPrint, indent);
      os.write(data ? "true" : "false", data ? 4 : 5);
    }
  }

  template<IsContainer T> static void _e_save(
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if (format == JSON) {
      os.put('[');
      bool first = true;
      for (const auto &elem: data) {
        if (!first)
          os.put(',');
        if (prettyPrint)
          os.put('\n');
        _e_save(elem, format, os, prettyPrint, indent + 1);
        first = false;
      }
      if (data.begin() != data.end()) {
        if (prettyPrint)
          os.put('\n');
        applyIndent(os, prettyPrint, indent);
      }
      os.put(']');
    }
  }

  template<typename T> static void serializeKey(const T &key, const Format format, std::ostringstream &os) {
    if constexpr (IsString<T>)
      os << key;
    else
      _e_save(key, format, os, false, 0);
  }

  template<IsMap T> static void _e_save(
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if (format == JSON) {
      os.put('{');
      bool first = true;
      for (const auto &[k, v]: data) {
        if (!first)
          os.put(',');
        if (prettyPrint)
          os.put('\n');
        applyIndent(os, prettyPrint, indent + 1);

        os.put('"');
        serializeKey(k, format, os);
        os.put('"');

        os.put(':');
        if (prettyPrint)
          os.put(' ');
        _e_save(v, format, os, prettyPrint, 0);
        first = false;
      }
      if (data.begin() != data.end()) {
        if (prettyPrint)
          os.put('\n');
        applyIndent(os, prettyPrint, indent);
      }
      os.put('}');
    }
  }

  template<typename U, typename V> static void _e_save(
    const std::pair<U, V> &data, const Format format, std::ostringstream &os, const bool prettyPrint,
    const int indent
  ) {
    if (format == JSON) {
      os.put('[');
      _e_save(data.first, format, os, prettyPrint, indent + 1);
      os.put(',');
      os.put(' ');
      _e_save(data.second, format, os, prettyPrint, indent + 1);
      applyIndent(os, prettyPrint, indent);
      os.put(']');
    }
  }

  template<typename... Ts>
  static void _e_save(
    const std::tuple<Ts...> &t, const Format format, std::ostringstream &os, const bool prettyPrint,
    const int indent
  ) {
    if (format == JSON) {
      os.put('[');
      bool first = true;
      std::apply(
        [&](const auto &... elems) {
          (([&] {
            if (!first)
              os.put(',');
            if (prettyPrint)
              os.put('\n');
            _e_save(elems, format, os, prettyPrint, indent + 2);
            first = false;
          }()), ...);
        }, t
      );
      if (sizeof...(Ts) > 0) {
        if (prettyPrint)
          os.put('\n');
        applyIndent(os, prettyPrint, indent + 1);
      }
      os.put(']');
    }
  }

  template<IsTuple T> static void _e_save(const T &t, Format format, std::ostringstream &os, int indent) {
    std::apply(
      [&](const auto &... elems) {
        _e_save(std::make_tuple(elems...), format, os, indent);
      }, t
    );
  }

  template<IsSmartPtr T> static void _e_save(
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if (data) {
      if constexpr (IsSTL<std::remove_cv_t<typename std::pointer_traits<T>::element_type>>) {
        _e_saveImpl(*data, format, prettyPrint, indent + 1, &os);
      } else {
        applyIndent(os, prettyPrint, indent);
        os.put('{');
        applyIndent(os, prettyPrint, indent);
        os.write(R"("type":)", 7);
        if (prettyPrint)
          os.put(' ');
        _e_save(data->ClassNameQualified(), format, os, prettyPrint, 0);
        os.put(',');
        os.write(R"("data":)", 7);
        if (prettyPrint)
          os.put(' ');
        data->_e_save(format, os, prettyPrint, indent + 1);
        applyIndent(os, prettyPrint, indent);
        os.put('}');
      }
    } else
      os << "null";
  }

  template<IsNotSTL T> static void _e_save(
    const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
  ) {
    if constexpr (requires {
      data._e_save(format, os, prettyPrint, indent + 1);
    }) {
      data._e_save(format, os, prettyPrint, indent + 1);
    } else {
      static_assert(
        _e_f<T>,
        "Missing save function. Ensure the type uses it's SERIALIZE_* macro."
      );
    }
  }
}

#endif //SAVE_HPP
