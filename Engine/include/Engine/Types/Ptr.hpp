//
// Ptr.hpp
// Author: Antoine Bastide
// Date: 14.09.2025
//

#ifndef PTR_HPP
#define PTR_HPP

namespace Engine2D {
  class Entity2D;
  class Component2D;
  class Scene;
}

namespace Engine {
  template<typename T> class Ptr final {
    public:
      using pointer = T *;
      using element_type = T;

      constexpr Ptr()
        : ptr(nullptr) {}

      constexpr Ptr(nullptr_t)
        : ptr(nullptr) {}

      constexpr Ptr(T *ptr)
        : ptr(ptr) {}

      ~Ptr() = default;

      Ptr(const Ptr &ptr)
        : ptr(ptr.ptr) {}

      Ptr(Ptr &&ptr) noexcept
        : ptr(ptr.ptr) {
        ptr.ptr = nullptr;
      }

      Ptr &operator=(const Ptr &other) {
        ptr = other.ptr;
        return *this;
      }

      Ptr &operator=(Ptr &&other) noexcept {
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
      }

      Ptr &operator=(T *other) {
        ptr = other;
        return *this;
      }

      void operator delete(void *ptr) = delete;
      void operator delete[](void *ptr) = delete;

      T *operator->() const {
        return ptr;
      }

      T &operator*() const {
        return *ptr;
      }

      constexpr T *get() const {
        return ptr;
      }

      constexpr bool valid() const {
        return ptr != nullptr;
      }

      constexpr operator bool() const {
        return ptr != nullptr;
      }

      constexpr auto operator<=>(const Ptr &other) const {
        return ptr <=> other.ptr;
      }

      constexpr bool operator==(const Ptr &other) const {
        return ptr == other.ptr;
      }

      constexpr auto operator<=>(const T *other) const {
        return ptr <=> other;
      }

      constexpr bool operator==(const T *other) const {
        return ptr == other;
      }

      constexpr bool operator==(std::nullptr_t) const {
        return ptr == nullptr;
      }
    private:
      T *ptr;
  };
} // Engine

template<typename T> struct std::hash<Engine::Ptr<T>> {
  constexpr size_t operator()(const Engine::Ptr<T> &ptr) const noexcept {
    return hash<T *>{}(ptr.get());
  }
};

#endif //PTR_HPP
