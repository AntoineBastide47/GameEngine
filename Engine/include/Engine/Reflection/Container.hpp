//
// Container.hpp
// Author: Antoine Bastide
// Date: 05.09.2025
//

#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <forward_list>

#include "Engine/Reflection/Concepts.hpp"

namespace Engine::Reflection {
  template<IsContainer T> static constexpr bool CanAddValue() {
    using Value = typename T::value_type;

    if constexpr (requires(T &container, Value &&v) { container.push_back(std::forward<Value>(v)); })
      return true;
    else if constexpr (requires(T &container, Value &&v) { container.insert(container.end(), std::forward<Value>(v)); })
      return true;
    else if constexpr (requires(T &container, Value &&v) { container.insert(std::forward<Value>(v)); })
      return true;
    else if constexpr (requires(T &container, Value &&v) { container.emplace_back(std::forward<Value>(v)); })
      return true;
    else if constexpr (std::is_same_v<T, std::forward_list<typename T::value_type>>)
      return true;
    else
      return false;
  }

  template<IsContainer T> static constexpr bool CanRemoveValue() {
    if constexpr (requires(T &container) { container.pop_back(); })
      return true;
    else if constexpr (requires { typename T::hasher; })
      return true;
    else if constexpr (requires(T &container) { container.erase(std::prev(container.end())); })
      return true;
    else if constexpr (requires(T &container) { container.erase(--container.end()); })
      return true;
    else if constexpr (std::is_same_v<T, std::forward_list<typename T::value_type>>)
      return true;
    else
      return false;
  }

  template<IsContainer T, typename Value> static void AddValue(T &container, Value &&value) {
    if constexpr (requires { container.push_back(std::forward<Value>(value)); })
      container.push_back(std::forward<Value>(value));
    else if constexpr (requires { container.insert(container.end(), std::forward<Value>(value)); })
      container.insert(std::end(container), std::forward<Value>(value));
    else if constexpr (requires { container.insert(std::forward<Value>(value)); })
      container.insert(std::forward<Value>(value));
    else if constexpr (requires { container.emplace_back(std::forward<Value>(value)); })
      container.emplace_back(std::forward<Value>(value));
    else if constexpr (std::is_same_v<T, std::forward_list<typename T::value_type>>)
      container.insert_after(container.before_begin(), std::move(value));
  }

  template<IsContainer T> static void RemoveValue(T &container) {
    if constexpr (requires { container.pop_back(); })
      container.pop_back();
    else if constexpr (requires { typename T::hasher; })
      container.erase(container.begin());
    else if constexpr (requires(T &container) { container.erase(std::prev(container.end())); })
      container.erase(std::prev(container.end()));
    else if constexpr (requires { container.erase(--container.end()); })
      container.erase(--container.end());
    else if constexpr (std::is_same_v<T, std::forward_list<typename T::value_type>>) {
      if (std::distance(container.begin(), container.end()) == 1)
        container.pop_front();
      else {
        auto it = container.before_begin();
        auto next = std::next(it);
        while (std::next(next) != container.end()) {
          ++it;
          ++next;
        }
        container.erase_after(it);
      }
    }
  }

  template<IsContainer T> static typename T::value_type ExtractValue(T &container) {
    using Value = typename T::value_type;

    if constexpr (requires { container.pop_back(); }) {
      // For vector, deque, etc.
      Value value = std::move(container.back());
      container.pop_back();
      return value;
    } else if constexpr (requires { typename T::hasher; }) {
      // For unordered containers
      if constexpr (requires { container.extract(container.begin()); }) {
        auto node = container.extract(container.begin());
        return std::move(node.value());
      } else {
        // Fallback for older standards
        auto it = container.begin();
        Value value = std::move(const_cast<Value &>(*it));
        container.erase(it);
        return value;
      }
    } else if constexpr (requires { container.extract(std::prev(container.end())); }) {
      // For set/map with extract support (C++17)
      auto node = container.extract(std::prev(container.end()));
      return std::move(node.value());
    } else if constexpr (requires(T &c) { c.erase(std::prev(c.end())); }) {
      // For list or older set/map without extract
      auto it = std::prev(container.end());
      if constexpr (std::is_const_v<std::remove_reference_t<decltype(*it)>>) {
        // For set/map where elements are const
        Value value = std::move(const_cast<Value &>(*it));
        container.erase(it);
        return value;
      } else {
        Value value = std::move(*it);
        container.erase(it);
        return value;
      }
    } else if constexpr (requires { container.erase(--container.end()); }) {
      auto it = --container.end();
      Value value = std::move(*it);
      container.erase(it);
      return value;
    } else if constexpr (std::is_same_v<T, std::forward_list<Value>>) {
      if (std::distance(container.begin(), container.end()) == 1) {
        Value value = std::move(container.front());
        container.pop_front();
        return value;
      }

      auto it = container.before_begin();
      auto next = std::next(it);
      while (std::next(next) != container.end()) {
        ++it;
        ++next;
      }
      Value value = std::move(*next);
      container.erase_after(it);
      return value;
    }

    return Value{};
  }
}

#endif //CONTAINER_HPP
