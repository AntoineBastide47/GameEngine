//
// Property.h
// Author: Antoine Bastide
// Date: 05/12/2024
//

#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>

#include "Log.h"

namespace Engine {
  /**
   * The Property class is a templated utility for encapsulating a value along with a callback mechanism that gets triggered
   * whenever the value changes. It allows you to define custom logic that executes whenever the property is updated.
   * @tparam T The type of the value stored in this property
   * @tparam Args The types of the arguments needed to call the callback function (optional)
   */
  template<typename T, typename... Args> class Property {
    public:
      using Callback = std::function<void(Args...)>;

      /**
       * @param initialValue The initial value to store in the property
       * @param callback The callback called when the value of this property is changed
       * @param args The arguments needed to call the callback function
       */
      Property(T initialValue, Callback callback, Args &&... args)
        : value(initialValue), callback(std::move(callback)), args(std::forward<Args>(args)...) {}

      /** Assignment operator */
      Property &operator=(const T &newValue) {
        value = newValue;
        notify();
        return *this;
      }

      /** Addition operator */
      Property &operator+=(const T &val) {
        return *this = value + val;
      }

      /** Subtraction operator */
      Property &operator-=(const T &val) {
        return *this = value - val;
      }

      /** Multiplication operator */
      Property &operator*=(const T &val) {
        return *this = value * val;
      }

      // ReSharper disable once CppNonExplicitConversionOperator
      operator T() const {
        return value;
      }

      /** Equality operator */
      bool operator==(const Property &other) const {
        return value == other.value;
      }

      /** Inequality operator */
      bool operator!=(const Property &other) const {
        return !(*this == other);
      }

      /** Forward operator to access data stored in the value of this property */
      T *operator->() {
        notify();
        return &value;
      }

      /** Forward operator to access data stored in the value of this property */
      const T *operator->() const {
        return &value;
      }
    private:
      T value;
      Callback callback;
      std::tuple<Args...> args;

      void notify() const {
        if (callback) {
          std::apply(callback, args);
        }
      }
  };
}

#endif //PROPERTY_H
