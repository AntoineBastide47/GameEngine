//
// Concepts.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <glm/glm.hpp>

#include "Engine/Types/Ptr.hpp"

namespace Engine {
  class JSON;
}

namespace Engine::Reflection {
  struct Reflectable;

  template<typename T> concept IsConst = std::is_const_v<std::remove_reference_t<T>>;

  template<typename T> concept IsNonOwningString =
      std::is_same_v<std::remove_cvref_t<T>, std::string_view> ||
      std::is_same_v<std::remove_cvref_t<T>, const char *> ||
      std::is_same_v<std::remove_cvref_t<T>, char *>;

  template<typename T> concept IsString = std::is_same_v<std::remove_cvref_t<T>, std::string> || IsNonOwningString<T>;

  template<typename T> concept IsNumber =
      std::is_arithmetic_v<std::remove_cvref_t<T>> &&
      !std::is_same_v<std::remove_cvref_t<T>, bool> &&
      !IsString<T>;

  template<typename> struct is_glm_vec234 : std::false_type {};

  template<glm::length_t N, typename T, glm::qualifier Q>
  struct is_glm_vec234<glm::vec<N, T, Q>> : std::bool_constant<N == 2 || N == 3 || N == 4> {};

  template<typename T>
  concept IsGlmVec234 = is_glm_vec234<std::remove_cvref_t<T>>::value;

  template<typename T> concept IsTuple = requires {
    std::tuple_size_v<std::remove_cvref_t<T>>;
  };

  template<typename T> concept IsPair =
      requires {
        typename std::remove_cvref_t<T>;
      } && std::is_same_v<
        std::remove_cvref_t<T>,
        std::pair<typename std::remove_cvref_t<T>::first_type, typename std::remove_cvref_t<T>::second_type>
      >;

  template<typename T> concept IsMap =
      requires {
        typename T::value_type;
        typename T::key_type;
        typename T::mapped_type;
        std::begin(std::declval<T &>());
        std::end(std::declval<T &>());
      } && IsPair<std::remove_cvref_t<typename T::value_type>>;

  template<typename T> concept IsContainer =
      requires {
        typename T::value_type;
        std::begin(std::declval<T &>());
        std::end(std::declval<T &>());
      } && !IsMap<T> && !IsString<T>;

  template<typename T> concept IsSharedPtr = requires(T ptr) {
    typename T::element_type;
    requires std::is_same_v<std::remove_cvref_t<T>, std::shared_ptr<typename T::element_type>>;
  };

  template<typename T> concept IsUniquePtr = requires(T ptr) {
    typename T::element_type;
    typename T::deleter_type;
    requires std::is_same_v<std::remove_cvref_t<T>,
      std::unique_ptr<typename T::element_type, typename T::deleter_type>>;
  };

  template<typename T> concept IsSmartPtr = IsSharedPtr<T> || IsUniquePtr<T>;

  template<typename T> concept IsEnum = std::is_enum_v<T>;

  template<typename T> concept IsReflectable = std::is_base_of_v<Reflectable, std::remove_cvref_t<T>>;
  template<typename T> concept IsNotReflectable = !IsReflectable<T>;

  template<typename T> concept IsReflectablePointer = std::is_same_v<std::remove_cvref_t<T>, Ptr<typename T::element_type>> && IsReflectable<typename T::element_type>;

  template<typename T> concept IsNonRecursive =
      IsNumber<T> || IsString<T> || std::is_same_v<std::remove_cvref_t<T>, bool> || IsGlmVec234<T> ||
      IsReflectablePointer<T>;

  enum Format {
    JSON, TEXT, BINARY
  };

  template<typename> static constexpr bool _e_f = false;

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
