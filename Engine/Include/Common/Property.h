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
   * whenever the value changes. It allows you to define custom logic that executes whenever the property is updated.<br>
   * The class has two callbacks available:
   *  - postCallback: The callback called once the value is set
   *  - preCallback: (Optional) The callback called before setting the value to pre-process it if needed
   * @tparam T The type of the value stored in this property
   */
  template<typename T> class Property {
    public:
      using PostCallback = std::function<void()>;
      using PreCallback = std::function<T(T value)>;

      /**
       * @param initialValue The initial value to store in the property
       * @param postCallback The callback called after value of this property is changed
       */
      explicit Property(T initialValue, const PostCallback &postCallback)
        : value(initialValue), preCallback(std::nullopt), postCallback(postCallback) {}

      /// Sets the preprocessing callback to the given callback
      void SetPreprocessingCallback(const PreCallback &preCallback) {
        this->preCallback = preCallback;
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

      /// Assignment operator
      Property &operator=(T newValue) {
        if (preCallback)
          newValue = (*preCallback)(newValue);
        value = newValue;
        postCallback();
        return *this;
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

      /// @returns The internal value of this property
      [[nodiscard]] T Get() const {
        return value;
      }

      /// Equality operator
      bool operator==(const Property &other) const {
        return value == other.value;
      }

      /// Equality operator
      bool operator==(const T value) {
        return this->value == value;
      }

      /// Equality operator
      friend bool operator==(const T value, const Property &prop) {
        return prop.value == value;
      }

      /// Inequality operator
      bool operator!=(const Property &other) const {
        return value != other.value;
      }

      /// Equality operator
      bool operator!=(const T value) {
        return this->value != value;
      }

      /// Equality operator
      friend bool operator!=(const T value, const Property &prop) {
        return prop.value != value;
      }

      /// Forward operator to access data stored in the value of this property
      T *operator->() {
        postCallback();
        return &value;
      }

      /// Forward operator to access data stored in the value of this property
      const T *operator->() const {
        postCallback();
        return &value;
      }
    private:
      /// The value stored in this property
      T value;
      /// The callback called before value is changed
      std::optional<PreCallback> preCallback;
      /// The callback called after value is changed
      PostCallback postCallback;
  };
}

#endif //PROPERTY_H
