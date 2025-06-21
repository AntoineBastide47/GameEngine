//
// Concepts.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <iterator>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Engine::Serialization {
  template<typename T> concept IsString =
      std::is_convertible_v<std::decay_t<T>, std::string_view> ||
      std::is_same_v<std::decay_t<T>, char> ||
      std::is_same_v<std::decay_t<T>, unsigned char> ||
      std::is_same_v<std::decay_t<T>, signed char>;

  template<typename T> concept IsNumber =
      std::is_arithmetic_v<std::decay_t<T>> &&
      !std::is_same_v<std::decay_t<T>, bool> &&
      !IsString<T>;

  template<typename T> struct is_pair_like : std::false_type {};

  template<typename A, typename B> struct is_pair_like<std::pair<A, B>> : std::true_type {};

  template<typename T> concept IsMap =
      requires {
        typename T::value_type;
        typename T::key_type;
        typename T::mapped_type;
        std::begin(std::declval<T &>());
        std::end(std::declval<T &>());
      } && is_pair_like<std::decay_t<typename T::value_type>>::value;

  template<typename T> concept IsContainer =
      requires {
        typename T::value_type;
        std::begin(std::declval<T &>());
        std::end(std::declval<T &>());
      } && !IsMap<T> && !IsString<T>;

  template<typename T> concept IsTuple = requires { typename std::tuple_size<std::remove_cvref_t<T>>::type; };

  template<typename T> concept IsSmartPtr = requires(T ptr) {
    typename T::element_type;
    requires std::is_same_v<T, std::shared_ptr<typename T::element_type>> ||
             std::is_same_v<T, std::unique_ptr<typename T::element_type>>;
  };

  template<typename T> concept IsNotSTL = requires(const T &data) {
    { data->GetClass() } -> std::convertible_to<std::string_view>;
  };
}

#endif //CONCEPTS_HPP
