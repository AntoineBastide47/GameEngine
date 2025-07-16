//
// JsonValue.hpp
// Author: Antoine Bastide
// Date: 05.07.2025
//

#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Engine {
  class JSON;

  /// Representation of a JSON null value
  struct null_t final {
    constexpr null_t() = default;
    explicit constexpr null_t(int) {}

    constexpr bool operator==(const null_t &) const {
      return true;
    }

    constexpr bool operator!=(const null_t &) const {
      return false;
    }
  };

  static inline constexpr null_t null{0};

  class JSON final {
    friend class JSONParser;
    public:
      using JSONArray = std::vector<JSON>;
      using JSONObject = std::unordered_map<std::string, JSON>;

      /// Creates a JSON null value
      JSON();
      /// Creates a JSON boolean value
      JSON(bool value);
      /// Creates a JSON number based on the given integer
      JSON(int value);
      /// Creates a JSON number based on the given double
      JSON(double value);
      /// Creates a JSON string based on the given C string
      explicit JSON(const char *value);
      /// Creates a JSON string based on the given C++ string
      explicit JSON(std::string value);
      // Creates either a JSON array or object based on the given values
      JSON(const std::initializer_list<JSON> &values);

      bool operator==(const JSON &other) const;
      bool operator!=(const JSON &other) const;

      bool operator<(const JSON &other) const;
      bool operator<=(const JSON &other) const;
      bool operator>(const JSON &other) const;
      bool operator>=(const JSON &other) const;

      /// Dump's this instance in the given ostream
      friend std::ostream &operator<<(std::ostream &os, const JSON &value);

      /// Load's this instance from the given istream
      friend std::istream &operator>>(std::istream &is, JSON &value);

      /// Assigns the given null value to this instance.
      JSON &operator=(nullptr_t value);

      /// Assigns the given null value to this instance.
      JSON &operator=(null_t value);

      /// Assigns the given boolean value to this instance.
      JSON &operator=(bool value);

      /// Assigns the given integer value to this instance.
      JSON &operator=(int value);

      /// Assigns the given double value to this instance.
      JSON &operator=(double value);

      /// Assigns the given C string value to this instance.
      JSON &operator=(const char *value);

      /// Assigns the given C++ string value to this instance.
      JSON &operator=(std::string value);

      /// Assigns the given array or object to this instance.
      JSON &operator=(const std::initializer_list<JSON> &values);

      /// @returns A reference to the JSON element at the specified index.
      /// @param index The position of the element to access within the array.
      /// @note If the current instance is not an array, it is converted into one. If the index is out of bounds, the array is resized to accommodate it.
      JSON &operator[](size_t index);

      /// @returns A const reference to the JSON element at the specified index.
      /// @param index The position of the element to access within the array.
      /// @note If the current instance is not an array, it is converted into one. If the index is out of bounds, the array is resized to accommodate it.
      const JSON &operator[](size_t index) const;

      /// @returns A reference to the JSON element at the specified key.
      /// @param key The key of the element to access within the object.
      /// @note If the current instance is not an object, it is converted into one.
      JSON &operator[](const std::string &key);

      /// @returns A const reference to the JSON element at the specified key.
      /// @param key The key of the element to access within the object.
      /// @note If the current instance is not an object, it is converted into one.
      const JSON &operator[](const std::string &key) const;

      template<typename T>
      /// @returns A reference to the internal data stored in this JSON value
      [[nodiscard]] T &Get() {
        if (!typeMatches<T>())
          throw std::logic_error(
            static_cast<std::string>("Can not convert from: ") + typeToString(type) + " to : " + typeid(T).name()
          );

        if constexpr (std::is_same_v<T, bool>)
          return std::get<bool>(data);
        else if constexpr (std::is_same_v<T, double>)
          return std::get<double>(data);
        else if constexpr (std::is_same_v<T, std::string>)
          return std::get<std::string>(data);
        else if constexpr (std::is_same_v<T, JSONArray>)
          return std::get<JSONArray>(data);
        else if constexpr (std::is_same_v<T, JSONObject>)
          return std::get<JSONObject>(data);
        else
          return std::get<null_t>(data);
      }

      template<typename T>
      /// @returns A const reference to the internal data stored in this JSON value
      [[nodiscard]] const T &Get() const {
        if (!typeMatches<T>())
          throw std::logic_error(
            static_cast<std::string>("Can not convert from: ") + typeToString(type) + " to : " + typeid(T).name()
          );

        if constexpr (std::is_same_v<std::remove_cvref_t<T>, bool>)
          return std::get<bool>(data);
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, double>)
          return std::get<double>(data);
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>)
          return std::get<std::string>(data);
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, JSONArray>)
          return std::get<JSONArray>(data);
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, JSONObject>)
          return std::get<JSONObject>(data);
        else
          return std::get<null_t>(data);
      }

      template<typename T>
      /// @returns A pointer to the internal data stored in this JSON value
      [[nodiscard]] T *TryGet() {
        if (!typeMatches<T>())
          return nullptr;
        return &Get<T>();
      }

      template<typename T>
      /// @returns A const pointer to the internal data stored in this JSON value
      [[nodiscard]] const T *TryGet() const {
        if (!typeMatches<T>())
          return nullptr;
        return &Get<T>();
      }

      template<typename T>
      /// @returns A reference to the internal data stored in this JSON value or the given default value
      [[nodiscard]] T &GetOr(const JSON &defaultValue) {
        if (typeMatches<T>())
          return Get<T>();

        return defaultValue.Get<T>();
      }

      template<typename T>
      /// @returns A const reference to the internal data stored in this JSON value or the given default value
      [[nodiscard]] const T &GetOr(const JSON &defaultValue) const {
        if (typeMatches<T>())
          return Get<T>();

        return defaultValue.Get<T>();
      }

      /// Appends a JSON value to this instance.
      /// @param value The JSON value to add.
      /// @note If this JSON is not of type array, it will be implicitly converted to a JSON array.
      void PushBack(const JSON &value);

      /// Appends multiple JSON values to this instance.
      /// @param values The JSON values to add.
      /// @note If this JSON is not of type array, it will be implicitly converted to a JSON array.
      void PushBackAll(const std::initializer_list<JSON> &values);

      /// Inserts a JSON value into this array at the specified index.
      /// @param index The position in the array where the value will be inserted.
      /// @param value The JSON value to insert
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      void Insert(size_t index, const JSON &value);

      /// Removes the JSON value at the given index from this instance.
      /// @param index The index at which to erase the JSON value.
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      void Erase(size_t index);

      /// Resizes this instance to the given size.
      /// @param size The new size of this instance.
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      void Resize(size_t size);

      /// @return A reference to the first element of this instance.
      /// @throws std::logic_error if this JSON is not of type array.
      JSON &Front();

      /// @return A const reference to the first element of this instance.
      /// @throws std::logic_error if this JSON is not of type array.
      const JSON &Front() const;

      /// @return A reference to the last element of this instance.
      /// @throws std::logic_error if this JSON is not of type array.
      JSON &Back();

      /// @return A const reference to the last element of this instance.
      /// @throws std::logic_error if this JSON is not of type array.
      const JSON &Back() const;

      /// @returns A reference to the element in this instance at the specified index.
      /// @param index The position of the element within the instance.
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      [[nodiscard]] JSON &At(size_t index);

      /// @returns A const reference to the element in this instance at the specified index.
      /// @param index The position of the element within the instance.
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      [[nodiscard]] const JSON &At(size_t index) const;

      /// @returns A reference to the element in this instance at the specified index or,
      /// the default value if it is null or if this instance isn't an array.
      /// @param index The position of the element within the instance.
      /// @param defaultValue The default value to return in case of errors.
      [[nodiscard]] JSON &Value(size_t index, JSON &defaultValue);

      /// @returns A const reference to the element in this instance at the specified index or,
      /// the default value if it is null or if this instance isn't an array.
      /// @param index The position of the element within the instance.
      /// @param defaultValue The default value to return in case of errors.
      [[nodiscard]] const JSON &Value(size_t index, JSON &defaultValue) const;

      /// @returns true if this instance contains the given JSON value, false if not.
      /// @param value The value to check the existence of.
      /// @throws std::logic_error if this JSON is not of type array or object.
      [[nodiscard]] bool Contains(const JSON &value) const;

      /// Assigns the specified value to the specified key in this instance.
      /// @note If this JSON is not of type object, it will be implicitly converted to a JSON object.
      /// @param key The key at which the value will be stored.
      /// @param value The values to store.
      void Emplace(const std::string &key, const JSON &value);

      /// Assigns the specified values to their specified keys in this instance.
      /// @note If this JSON is not of type object, it will be implicitly converted to a JSON object.
      /// @param pairs The pairs of key-value to add to this instance.
      void EmplaceAll(const std::initializer_list<std::pair<std::string, JSON>> &pairs);

      /// Removes the JSON value assigned to the given key from this instance.
      /// @param key The key at which to erase the JSON value.
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      void Erase(const std::string &key);

      /// Removes the JSON values assigned to the given keys from this instance.
      /// @param keys The keys at which to erase the JSON value.
      /// @throws std::out_of_range if index exceeds the current array size.
      /// @throws std::logic_error if this JSON is not of type array.
      void EraseAll(const std::initializer_list<std::string> &keys);

      /// @returns A reference to the element in this instance at the specified key.
      /// @param key The key of the element within the instance.
      /// @throws std::out_of_range if key does not exist in the current object.
      /// @throws std::logic_error if this JSON is not of type array.
      [[nodiscard]] JSON &At(const std::string &key);

      /// @returns A const reference to the element in this instance at the specified key.
      /// @param key The key of the element within the instance.
      /// @throws std::out_of_range if key does not exist in the current object.
      /// @throws std::logic_error if this JSON is not of type array.
      [[nodiscard]] const JSON &At(const std::string &key) const;

      /// @returns A reference to the element in this instance at the specified key or,
      /// the default value if it is null or if this instance isn't an array.
      /// @param key The key of the element within the instance.
      /// @param defaultValue The default value to return in case of errors.
      [[nodiscard]] JSON &Value(const std::string &key, JSON &defaultValue);

      /// @returns A const reference to the element in this instance at the specified key or,
      /// the default value if it is null or if this instance isn't an array.
      /// @param key The key of the element within the instance.
      /// @param defaultValue The default value to return in case of errors.
      [[nodiscard]] const JSON &Value(const std::string &key, JSON &defaultValue) const;

      /// @returns true if this instance contains the given key, false if not.
      /// @param key The key to check the existence of.
      /// @throws std::logic_error if this JSON is not of type object.
      [[nodiscard]] bool Contains(const std::string &key) const;

      /// @returns The size of this instance.
      /// @throws std::logic_error if this JSON is not of type array or object.
      [[nodiscard]] size_t Size() const;

      /// @returns true if this instance is empty, false if not.
      /// @throws std::logic_error if this JSON is not of type array or object.
      [[nodiscard]] bool Empty() const;

      /// Clears all the values contained in this instance.
      void Clear();

      /// @returns true if this instance in a JSON null value.
      [[nodiscard]] bool IsNull() const;

      /// @returns true if this instance in a JSON boolean value.
      [[nodiscard]] bool IsBool() const;

      /// @returns true if this instance in a JSON number.
      [[nodiscard]] bool IsNumber() const;

      /// @returns true if this instance in a JSON string.
      [[nodiscard]] bool IsString() const;

      /// @returns true if this instance in a JSON array.
      [[nodiscard]] bool IsArray() const;

      /// @returns true if this instance in a JSON object.
      [[nodiscard]] bool IsObject() const;

      /// @returns A stringified representation of this instance
      /// @param indentSize Number of characters per indentation level; set to -1 for a compact form
      /// @param indentChar Character used for indentation
      [[nodiscard]] std::string Dump(int indentSize = -1, char indentChar = ' ') const;

      /// @returns A new JSON array containing the given values
      /// @param values The optional values to add to this array
      [[nodiscard]] static JSON Array(const std::initializer_list<JSON> &values = {});

      /// @returns A new JSON object containing the given keys and values
      /// @param values The optional keys and values to add to this array
      [[nodiscard]] static JSON Object(const std::initializer_list<std::pair<std::string, JSON>> &values = {});
    private:
      enum JSONType {
        Null = 0, Boolean, Number, String, array, object
      };

      JSONType type;
      std::variant<null_t, bool, double, std::string, JSONArray, JSONObject> data;

      /// @returns true if the instance is an array or an object, false if not
      [[nodiscard]] bool isComplexType() const;

      template<typename T> bool typeMatches() const {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, bool>)
          return type == Boolean;
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, double>)
          return type == Number;
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>)
          return type == String;
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, JSONArray>)
          return type == array;
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, JSONObject>)
          return type == object;
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, null_t>)
          return type == Null;
        return false;
      }

      inline static constexpr const char *typeToString(const JSONType type) {
        switch (type) {
          case Null:
            return "null";
          case Boolean:
            return "boolean";
          case Number:
            return "number";
          case String:
            return "string";
          case array:
            return "array";
          case object:
            return "object";
        }
        return "unknown";
      }
  };
}

#endif //JSON_HPP
