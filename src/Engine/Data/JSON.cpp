//
// JsonValue.cpp
// Author: Antoine Bastide
// Date: 05.07.2025
//

#include <sstream>

#include "Engine/Data/JSON.hpp"
#include "Engine/Data/JsonParser.hpp"

namespace Engine {
  JSON::JSON()
    : type(Null), data(null) {}

  auto JSONNull = JSON();

  JSON::JSON(const bool value)
    : type(Boolean), data(value) {}

  JSON::JSON(const int value)
    : type(Number), data(static_cast<double>(value)) {}

  JSON::JSON(const double value)
    : type(Number), data(value) {}

  JSON::JSON(const char *value)
    : JSON(std::string(value)) {}

  JSON::JSON(std::string value)
    : type(String), data(std::move(value)) {}

  JSON::JSON(const std::initializer_list<JSON> &values) {
    const bool isObject = std::ranges::all_of(
      values, [](const JSON &j) {
        return j.IsArray() && j.Size() == 2 && j[0].IsString();
      }
    );

    type = isObject ? object : array;

    if (isObject) {
      JSONObject obj;
      for (const auto &pair: values) {
        obj.emplace(pair[0].Get<std::string>(), pair[1]);
      }
      data = std::move(obj);
    } else
      data = values;
  }

  bool JSON::operator==(const JSON &other) const {
    return type == other.type && data == other.data;
  }

  bool JSON::operator!=(const JSON &other) const {
    return !(*this == other);
  }

  bool JSON::operator<(const JSON &other) const {
    return type < other.type;
  }

  bool JSON::operator<=(const JSON &other) const {
    return type <= other.type;
  }

  bool JSON::operator>(const JSON &other) const {
    return type > other.type;
  }

  bool JSON::operator>=(const JSON &other) const {
    return type >= other.type;
  }

  std::ostream &operator<<(std::ostream &os, const JSON &value) {
    return os << value.Dump();
  }

  std::istream &operator>>(std::istream &is, JSON &value) {
    value = JSONParser(is).Parse();
    return is;
  }

  JSON &JSON::operator=(const nullptr_t) {
    type = Null;
    data = null;
    return *this;
  }

  JSON &JSON::operator=(const null_t) {
    type = Null;
    data = null;
    return *this;
  }

  JSON &JSON::operator=(const bool value) {
    type = Boolean;
    data = value;
    return *this;
  }

  JSON &JSON::operator=(const int value) {
    type = Number;
    data = static_cast<double>(value);
    return *this;
  }

  JSON &JSON::operator=(double value) {
    type = Number;
    data = value;
    return *this;
  }

  JSON &JSON::operator=(const char *value) {
    return operator=(std::string(value));
  }

  JSON &JSON::operator=(std::string value) {
    type = String;
    data = std::move(value);
    return *this;
  }

  JSON &JSON::operator=(const std::initializer_list<JSON> &values) {
    const bool isObject = std::ranges::all_of(
      values, [](const JSON &j) {
        return j.IsArray() && j.Size() == 2 && j[0].IsString();
      }
    );

    type = isObject ? object : array;

    if (isObject) {
      JSONObject obj;
      for (const auto &pair: values) {
        obj.emplace(pair[0].Get<std::string>(), pair[1]);
      }
      data = std::move(obj);
    } else
      data = values;

    return *this;
  }

  JSON &JSON::operator[](const size_t index) {
    if (type != array) {
      type = array;
      data = JSONArray{};
    }

    auto &arr = std::get<JSONArray>(data);
    if (index >= arr.size())
      arr.resize(index + 1);

    return arr[index];
  }

  const JSON &JSON::operator[](const size_t index) const {
    if (type == array) {
      if (const auto &array = Get<JSONArray>(); index < array.size())
        return array[index];
      throw std::out_of_range("JSON::operator[]");
    }
    throw std::logic_error("JSON::operator[] on a non array-type");
  }

  JSON &JSON::operator[](const std::string &key) {
    if (type != object) {
      type = object;
      data = JSONObject{};
    }
    auto &obj = std::get<JSONObject>(data);
    return obj[key];
  }

  const JSON &JSON::operator[](const std::string &key) const {
    if (type == object) {
      if (const auto &obj = Get<JSONObject>(); obj.contains(key))
        return obj.at(key);
      throw std::out_of_range("JSON::operator[]");
    }
    throw std::logic_error("JSON::operator[] on a non object-type");
  }

  void JSON::PushBack(const JSON &value) {
    if (type != array) {
      type = array;
      data = JSONArray{};
    }
    std::get<JSONArray>(data).push_back(value);
  }

  void JSON::PushBackAll(const std::initializer_list<JSON> &values) {
    if (type != array) {
      type = array;
      data = JSONArray{};
    }
    auto &array = std::get<JSONArray>(data);
    array.insert(array.end(), values.begin(), values.end());
  }

  void JSON::Erase(const size_t index) {
    if (type == array) {
      if (auto &array = std::get<JSONArray>(data); index >= array.size())
        throw std::out_of_range("JSON::Erase");
      else
        std::get<JSONArray>(data).erase(array.begin() + index);
    }
    throw std::logic_error("JSON::Erase on a non array-type");
  }

  void JSON::Resize(const size_t size) {
    if (type != array) {
      type = array;
      data = JSONArray{};
    }
    std::get<JSONArray>(data).resize(size);
  }

  JSON &JSON::Front() {
    if (type == array)
      return std::get<JSONArray>(data).front();
    throw std::logic_error("JSON::Front() called on non-array type");
  }

  const JSON &JSON::Front() const {
    if (type == array)
      return std::get<JSONArray>(data).front();
    throw std::logic_error("JSON::Front() called on non-array type");
  }

  JSON &JSON::Back() {
    if (type == array)
      return std::get<JSONArray>(data).back();
    throw std::logic_error("JSON::Back called on non-array type");
  }

  const JSON &JSON::Back() const {
    if (type == array)
      return std::get<JSONArray>(data).back();
    throw std::logic_error("JSON::Back called on non-array type");
  }

  JSON &JSON::At(const size_t index) {
    if (type == array)
      return std::get<JSONArray>(data).at(index);
    throw std::out_of_range("JSON::At() on a non array-type");
  }

  const JSON &JSON::At(const size_t index) const {
    if (type == array)
      return std::get<JSONArray>(data).at(index);
    throw std::out_of_range("JSON::At() on a non array-type");
  }

  JSON &JSON::Value(const size_t index, JSON &defaultValue) {
    if (type == array) {
      auto &array = Get<JSONArray>();
      return array[index] == JSON{} ? defaultValue : array[index];
    }
    return defaultValue;
  }

  const JSON &JSON::Value(const size_t index, JSON &defaultValue) const {
    if (type == array) {
      const auto &array = Get<JSONArray>();
      return array[index] == JSON{} ? defaultValue : array[index];
    }
    return defaultValue;
  }

  void JSON::Insert(const size_t index, const JSON &value) {
    if (type == array) {
      if (const auto &array = std::get<JSONArray>(data); index >= array.size())
        throw std::out_of_range("JSON::Insert()");
      std::get<JSONArray>(data).at(index) = value;
    }
    throw std::logic_error("JSON::Insert() called on non-array type");
  }

  void JSON::Emplace(const std::string &key, const JSON &value) {
    if (type != object) {
      type = object;
      data = JSONObject{};
    }
    std::get<JSONObject>(data)[key] = value;
  }

  void JSON::EmplaceAll(const std::initializer_list<std::pair<std::string, JSON>> &pairs) {
    if (type != object) {
      type = object;
      data = JSONObject{};
    }
    std::get<JSONObject>(data).insert(pairs.begin(), pairs.end());
  }

  void JSON::Erase(const std::string &key) {
    if (type == object)
      std::get<JSONObject>(data).erase(key);
  }

  void JSON::EraseAll(const std::initializer_list<std::string> &keys) {
    if (type == object)
      for (const auto &key: keys)
        Erase(key);
  }

  JSON &JSON::At(const std::string &key) {
    if (type == object)
      return std::get<JSONObject>(data).at(key);
    throw std::out_of_range("JSON::At() on a non object-type");
  }

  const JSON &JSON::At(const std::string &key) const {
    if (type == object)
      return std::get<JSONObject>(data).at(key);
    throw std::out_of_range("JSON::At() on a non object-type");
  }

  JSON &JSON::Value(const std::string &key, JSON &defaultValue) {
    if (type == object) {
      auto &obj = Get<JSONObject>();
      return obj.at(key) == JSON{} ? defaultValue : obj.at(key);
    }
    return defaultValue;
  }

  const JSON &JSON::Value(const std::string &key, JSON &defaultValue) const {
    if (type == object) {
      const auto &obj = Get<JSONObject>();
      return obj.at(key) == JSON{} ? defaultValue : obj.at(key);
    }
    return defaultValue;
  }

  bool JSON::Contains(const std::string &key) const {
    if (type == object)
      return Get<JSONObject>().contains(key);
    throw std::out_of_range("JSON::Size() called on non-object type");
  }

  bool JSON::Contains(const JSON &value) const {
    if (type == object) {
      const auto &obj = Get<JSONObject>();
      return std::ranges::find_if(
               obj, [&](const auto &pair) {
                 return pair.second == value;
               }
             ) != obj.end();
    }

    if (type == array) {
      const auto &array = Get<JSONArray>();
      return std::ranges::find(array, value) != array.end();
    }

    throw std::out_of_range("JSON::Contains() called on non-array or non-object type");
  }

  size_t JSON::Size() const {
    if (type == array)
      return Get<JSONArray>().size();
    if (type == object)
      return Get<JSONObject>().size();
    throw std::out_of_range("JSON::Size() called on non-array or non-object type");
  }

  bool JSON::Empty() const {
    if (type == array)
      return Get<JSONArray>().empty();
    if (type == object)
      return Get<JSONObject>().empty();
    throw std::out_of_range("JSON::Empty() called on non-array or non-object type");
  }

  void JSON::Clear() {
    if (type == array)
      std::get<JSONArray>(data).clear();
    else if (type == object)
      std::get<JSONObject>(data).clear();
  }

  bool JSON::IsNull() const {
    return type == Null;
  }

  bool JSON::IsBool() const {
    return type == Boolean;
  }

  bool JSON::IsNumber() const {
    return type == Number;
  }

  bool JSON::IsString() const {
    return type == String;
  }

  bool JSON::IsArray() const {
    return type == array;
  }

  bool JSON::IsObject() const {
    return type == object;
  }

  std::string JSON::Dump(const int indentSize, const char indentChar) const {
    std::ostringstream oss;
    const bool prettyPrint = indentSize > -1;
    const int newIndent = prettyPrint ? indentSize + 1 : -1;
    const int endCharIndent = indentSize > 0 ? indentSize : 0;

    switch (type) {
      case Null:
        oss.write("null", 4);
        break;
      case Boolean: {
        const auto &value = Get<bool>();
        oss.write(value ? "true" : "false", value ? 4 : 5);
        break;
      }
      case Number: {
        const auto value = Get<double>();
        oss << value << (static_cast<int>(value) == value ? ".0" : "");
        break;
      }
      case String: {
        const auto &value = Get<std::string>();
        oss.put('"');
        oss.write(value.c_str(), value.size());
        oss.put('"');
        break;
      }
      case array: {
        std::string dump;
        oss.put('[');
        for (const auto &item: std::get<JSONArray>(data)) {
          if (prettyPrint) {
            oss.put('\n');
            oss.write(std::string(newIndent * 2, indentChar).c_str(), newIndent * 2);
          }
          dump = item.Dump(newIndent, indentChar);
          oss.write(dump.c_str(), dump.size());
          oss.put(',');
          if (prettyPrint)
            oss.put(' ');
        }
        if (!std::get<JSONArray>(data).empty()) {
          oss.seekp(prettyPrint ? -2 : -1, std::ios_base::end);
          if (prettyPrint)
            oss.put('\n');
        }
        if (prettyPrint)
          oss.write(std::string(endCharIndent * 2, indentChar).c_str(), endCharIndent * 2);
        oss.put(']');
        break;
      }
      case object: {
        std::string dump;
        oss.put('{');
        for (const auto &[key, value]: std::get<JSONObject>(data)) {
          if (prettyPrint) {
            oss.put('\n');
            oss.write(std::string(newIndent * 2, indentChar).c_str(), newIndent * 2);
          }
          oss.put('"');
          oss.write(key.c_str(), key.size());
          oss.put('"');
          oss.put(':');
          if (prettyPrint)
            oss.put(' ');
          dump = value.Dump(newIndent, indentChar);
          oss.write(dump.c_str(), dump.size());
          oss.put(',');
        }
        if (!std::get<JSONObject>(data).empty()) {
          oss.seekp(-1, std::ios_base::end);
          if (prettyPrint)
            oss.put('\n');
        }
        if (prettyPrint)
          oss.write(std::string(endCharIndent * 2, indentChar).c_str(), endCharIndent * 2);
        oss.put('}');
        break;
      }
    }

    return oss.str();
  }

  JSON JSON::Array(const std::initializer_list<JSON> &values) {
    JSON json;
    json.type = array;
    json.data = values;
    return json;
  }

  JSON JSON::Object(const std::initializer_list<std::pair<std::string, JSON>> &values) {
    JSON json;
    json.type = object;
    json.data = JSONObject(values.begin(), values.end());
    return json;
  }

  bool JSON::isComplexType() const {
    return type == object || type == array;
  }
}
