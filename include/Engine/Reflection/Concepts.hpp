//
// Concepts.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <iterator>
#include <memory>
#include <sstream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Engine {
  class JSON;
}

namespace Engine::Reflection {
  class Reflectable;

  template<typename T> concept IsString =
      std::is_same_v<std::remove_cvref_t<T>, std::string> ||
      std::is_same_v<std::remove_cvref_t<T>, std::string_view> ||
      std::is_same_v<std::remove_cvref_t<T>, const char *> ||
      std::is_same_v<std::remove_cvref_t<T>, char *>;

  template<typename T> concept IsNumber =
      std::is_arithmetic_v<std::remove_cvref_t<T>> &&
      !std::is_same_v<std::remove_cvref_t<T>, bool> &&
      !IsString<T>;

  template<typename T> struct IsPair : std::false_type {};

  template<typename A, typename B> struct IsPair<std::pair<A, B>> : std::true_type {};

  template<typename T> concept IsMap =
      requires {
        typename T::value_type;
        typename T::key_type;
        typename T::mapped_type;
        std::begin(std::declval<T &>());
        std::end(std::declval<T &>());
      } && IsPair<std::remove_cvref_t<typename T::value_type>>::value;

  template<typename T> concept IsContainer =
      requires {
        typename T::value_type;
        std::begin(std::declval<T &>());
        std::end(std::declval<T &>());
      } && !IsMap<T> && !IsString<T>;

  template<typename T> concept IsTuple = requires { typename std::tuple_size<std::remove_cvref_t<T>>::type; };

  template<typename T> concept IsSharedPtr = requires(T ptr) {
    typename T::element_type;
    requires std::is_same_v<std::remove_cvref_t<T>, std::shared_ptr<typename T::element_type>>;
  };

  template<typename T> concept IsUniquePtr = requires(T ptr) {
    typename T::element_type;
    requires std::is_same_v<std::remove_cvref_t<T>, std::unique_ptr<typename T::element_type>>;
  };

  template<typename T> concept IsSmartPtr = IsSharedPtr<T> || IsUniquePtr<T>;

  template<typename T> concept IsNotSTL = std::is_base_of_v<Reflectable, std::remove_cvref_t<T>>;
  template<typename T> concept IsSTL = !IsNotSTL<T>;

  enum Format {
    JSON, TEXT, BINARY
  };

  /// How many spaces to use for JSON pretty printing, set to 2 by default
  static inline unsigned int JsonIndentSize = 2;

  static inline void applyIndent(std::ostringstream &os, const bool prettyPrint, const int indent) {
    if (prettyPrint)
      os.write(std::string(indent * JsonIndentSize, ' ').c_str(), indent * JsonIndentSize);
  }

  template<typename> static constexpr bool _e_f = false;

  // detection trait
  template<typename, typename = void>
  struct hasSaveFunction : std::false_type {};

  template<typename U>
  struct hasSaveFunction<
        U,
        std::void_t<
          decltype(
            true
              ? _e_save(
                std::declval<U>(),
                std::declval<Format>(),
                std::declval<std::ostringstream &>(),
                std::declval<bool>(),
                std::declval<int>()
              )
              : std::declval<const U>()._e_save(
                std::declval<Format>(),
                std::declval<std::ostringstream &>(),
                std::declval<bool>(),
                std::declval<int>()
              )
          )
        >
      > : std::true_type {};

  template<typename, typename = void>
  struct hasLoadFunction : std::false_type {};

  template<typename U>
  struct hasLoadFunction<
        U,
        std::void_t<
          decltype(
            true
              ? _e_load(
                std::declval<U &>(),
                std::declval<Format>(),
                std::declval<const Engine::JSON &>()
              )
              : std::declval<U>()._e_load(
                std::declval<Format>(),
                std::declval<const Engine::JSON &>()
              )
          )
        >
      > : std::true_type {};

  template<typename T, typename Format>
  concept HasFreeSave = requires(T &data, Format fmt, Engine::JSON &json) {
    { _e_save(data, fmt, json) } -> std::same_as<void>;
  };

  template<typename T, typename Format>
  concept HasMemberSave = requires(const T &obj, Format fmt, Engine::JSON &json) {
    { obj._e_save(fmt, json) } -> std::same_as<void>;
  };

  template<typename T, typename Format>
  concept HasSaveFunction = HasFreeSave<T, Format> || HasMemberSave<T, Format>;

  template<typename T, typename Format>
  concept HasFreeLoad = requires(T &t, Format fmt, const Engine::JSON &j) {
    { _e_load(t, fmt, j) } -> std::same_as<void>;
  };

  template<typename T, typename Format>
  concept HasMemberLoad = requires(T &obj, Format fmt, const Engine::JSON &j) {
    { obj._e_load(fmt, j) } -> std::same_as<void>;
  };

  template<typename T, typename Format>
  concept HasLoadFunction = HasFreeLoad<T, Format> || HasMemberLoad<T, Format>;
}

#endif //CONCEPTS_HPP
