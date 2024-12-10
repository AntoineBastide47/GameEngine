//
// Property.h
// Author: Antoine Bastide
// Date: 05/12/2024
//

#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>

namespace Engine {
  /**
   * The Property class is a templated utility for encapsulating a value along with a callback mechanism that gets triggered
   * whenever the value changes. It allows you to define custom logic that executes whenever the property is updated.
   * @tparam T The type of the value stored in this property
   */
  template<typename T> class Property {
    public:
      using Callback = std::function<void()>;

      /**
       * @param initialValue The initial value to store in the property
       * @param callback The callback called when the value of this property is changed
       */
      Property(T initialValue, Callback callback) : value(initialValue), callback(std::move(callback)) {}

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

      [[nodiscard]] T Get() const {
        return value;
      }

      /** Equality operator */
      bool operator==(const Property &other) const {
        return value == other.value;
      }

      /** Inequality operator */
      bool operator!=(const Property &other) const {
        return value != other.value;
      }

      /** Equality operator */
      bool operator==(const T &other) const {
        return value == other;
      }

      /** Inequality operator */
      bool operator!=(const T &other) const {
        return value != other;
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

      void notify() const {
        if (callback)
          callback();
      }
  };
}

#endif //PROPERTY_H
