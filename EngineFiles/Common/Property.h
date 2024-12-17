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
       * @param postCallback The callback called after value of this property is changed
       * @param preCallback The callback called before value of this property is changed
       */
      explicit Property(
        T initialValue, const Callback &postCallback, const std::optional<Callback> &preCallback = std::nullopt
      ) : value(initialValue), preCallback(preCallback), postCallback(postCallback) {}

      /// Assignment operator
      Property &operator=(const T &newValue) {
        if (preCallback)
          (*preCallback)();
        value = newValue;
        postCallback();
        return *this;
      }

      T operator+(const Property &prop) const {
        return value + prop.value;
      }

      T operator+(const T &other) const {
        return value + other;
      }

      friend T operator+(const T &other, const Property &prop) {
        return other + prop.value;
      }

      T operator-(const Property &prop) const {
        return value - prop.value;
      }

      T operator-(const T &other) const {
        return value - other;
      }

      friend T operator-(const T &other, const Property &prop) {
        return other - prop.value;
      }

      T operator*(const Property &prop) const {
        return value * prop.value;
      }

      T operator*(const T &other) const {
        return value * other;
      }

      friend T operator*(const T &other, const Property &prop) {
        return other * prop.value;
      }

      T operator/(const Property &prop) const {
        return value / prop.value;
      }

      T operator/(const T &other) const {
        return value / other;
      }

      friend T operator/(const T &other, const Property &prop) {
        return other / prop.value;
      }

      /// Addition operator
      Property &operator+=(const T &val) {
        return *this = value + val;
      }

      /// Subtraction operator
      Property &operator-=(const T &val) {
        return *this = value - val;
      }

      /// Multiplication operator
      Property &operator*=(const T &val) {
        return *this = value * val;
      }

      /// Division operator
      Property &operator/=(const T &val) {
        return *this = value / val;
      }

      operator T() const {
        return value;
      }

      /// Equality operator
      bool operator==(const Property &other) const {
        return value == other.value;
      }

      /// Inequality operator
      bool operator!=(const Property &other) const {
        return value != other.value;
      }

      /// Equality operator
      bool operator==(const T &other) const {
        return value == other;
      }

      /// Inequality operator
      bool operator!=(const T &other) const {
        return value != other;
      }

      /// Forward operator to access data stored in the value of this property
      T *operator->() {
        if (preCallback)
          (*preCallback)();
        postCallback();
        return &value;
      }

      /// Forward operator to access data stored in the value of this property
      const T *operator->() const {
        if (preCallback)
          (*preCallback)();
        postCallback();
        return &value;
      }
    private:
      /// The value stored in this property
      T value;
      /// The callback called before value is changed
      std::optional<Callback> preCallback;
      /// The callback called after value is changed
      Callback postCallback;
  };
}

#endif //PROPERTY_H
