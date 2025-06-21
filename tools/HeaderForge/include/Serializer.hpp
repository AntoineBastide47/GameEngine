//
// Serializer.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <sstream>
#include <string>

#include "Concepts.hpp"

#define _e_SERIALIZER_HEADER_INJECT_START
#define _e_SERIALIZER_HEADER_INJECT_END

namespace Engine::Serialization {
  class Serializer final {
    enum Format {
      JSON, TEXT, BINARY
    };
    public:
      /// How many spaces to use for JSON pretty printing, set to 2 by default
      static unsigned int JsonIndentSize;

      /**
       * Converts the given data to a JSON representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @param prettyPrint If the JSON string needs to be pretty printed, false by default
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToJson(const T &data, const bool prettyPrint = false) {
        return saveImpl(data, JSON, prettyPrint);
      }

      /**
       * Converts the given data to a text representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToText(const T &data) {
        throw std::logic_error("Operation not yet supported");
      }

      /**
       * Converts the given data to a binary representation
       * @tparam T The type of the data
       * @param data The data to convert
       * @note Throws a compile time error when trying to save an STL type that isn't supported, or a non STL type not marked as serializable
       * @return The converted data
       */
      template<typename T> static std::string ToBinary(const T &data) {
        throw std::logic_error("Operation not yet supported");
      }
    private:
      static void applyIndent(std::ostringstream &os, const bool prettyPrint, const int indent) {
        if (prettyPrint)
          os.write(std::string(indent * JsonIndentSize, ' ').c_str(), indent * JsonIndentSize);
      }

      template<typename T> static std::string saveImpl(
        const T &data, Format format, const bool prettyPrint, const int indent = 0
      ) {
        std::ostringstream os;
        save(data, format, os, prettyPrint, indent);
        return os.str();
      }

      template<IsNumber T> static void save(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        if (format == JSON) {
          applyIndent(os, prettyPrint, indent);
          os << data;
        }
      }

      template<IsString T> static void save(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        if (format == JSON) {
          applyIndent(os, prettyPrint, indent);
          os.put('"');
          os << data;
          os.put('"');
        }
      }

      template<typename T> requires std::is_same_v<std::decay_t<T>, bool>
      static void save(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        if (format == JSON) {
          applyIndent(os, prettyPrint, indent);
          os.write(data ? "true" : "false", data ? 4 : 5);
        }
      }

      template<IsContainer T> static void save(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        if (format == JSON) {
          os.put('[');
          bool first = true;
          for (const auto &elem: data) {
            if (!first)
              os.put(',');
            if (prettyPrint)
              os.put('\n');
            save(elem, format, os, prettyPrint, indent + 1);
            first = false;
          }
          if (data.begin() != data.end()) {
            if (prettyPrint)
              os.put('\n');
            applyIndent(os, prettyPrint, indent);
          }
          os.put(']');
        }
      }

      template<typename T> static void serializeKey(const T &key, const Format format, std::ostringstream &os) {
        if constexpr (IsString<T>)
          os << key;
        else
          save(key, format, os, false, 0);
      }

      template<IsMap T> static void save(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        if (format == JSON) {
          os.put('{');
          bool first = true;
          for (const auto &[k, v]: data) {
            if (!first)
              os.put(',');
            if (prettyPrint)
              os.put('\n');
            applyIndent(os, prettyPrint, indent + 1);

            os.put('"');
            serializeKey(k, format, os);
            os.put('"');

            os.put(':');
            if (prettyPrint)
              os.put(' ');
            save(v, format, os, prettyPrint, 0);
            first = false;
          }
          if (data.begin() != data.end()) {
            if (prettyPrint)
              os.put('\n');
            applyIndent(os, prettyPrint, indent);
          }
          os.put('}');
        }
      }

      template<typename U, typename V> static void save(
        const std::pair<U, V> &data, const Format format, std::ostringstream &os, const bool prettyPrint,
        const int indent
      ) {
        if (format == JSON) {
          os.put('[');
          save(data.first, format, os, prettyPrint, indent + 1);
          os.put(',');
          os.put(' ');
          save(data.second, format, os, prettyPrint, indent + 1);
          applyIndent(os, prettyPrint, indent);
          os.put(']');
        }
      }

      template<typename... Ts>
      static void save(
        const std::tuple<Ts...> &t, const Format format, std::ostringstream &os, const bool prettyPrint,
        const int indent
      ) {
        if (format == JSON) {
          os.put('[');
          bool first = true;
          std::apply(
            [&](const auto &... elems) {
              (([&] {
                if (!first)
                  os.put(',');
                if (prettyPrint)
                  os.put('\n');
                save(elems, format, os, prettyPrint, indent + 2);
                first = false;
              }()), ...);
            }, t
          );
          if (sizeof...(Ts) > 0) {
            if (prettyPrint)
              os.put('\n');
            applyIndent(os, prettyPrint, indent + 1);
          }
          os.put(']');
        }
      }

      template<IsTuple T> static void save(const T &t, Format format, std::ostringstream &os, int indent) {
        std::apply(
          [&](const auto &... elems) {
            save(std::make_tuple(elems...), format, os, indent);
          }, t
        );
      }

      template<IsSmartPtr T> static void saveSTL(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        save(*data, format, os, prettyPrint, indent);
      }

      template<IsSmartPtr T> static void saveNonSTL(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        applyIndent(os, prettyPrint, indent);
        os.put('{');
        applyIndent(os, prettyPrint, indent);
        os.write(R"("type":)", 7);
        if (prettyPrint)
          os.put(' ');
        save(data->GetClass(), format, os, prettyPrint, 0);
        os.put(',');
        os.write(R"("data":)", 7);
        if (prettyPrint)
          os.put(' ');

        switch (data->TypeId()) {
            #define _e_SERIALIZER_POINTER_INJECT_START
            #define _e_SERIALIZER_POINTER_INJECT_END
          default:
            save(*data, format, os, prettyPrint, indent);
            break;
        }

        applyIndent(os, prettyPrint, indent);
        os.put('}');
      }

      template<IsSmartPtr T> static void save(
        const T &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent
      ) {
        if (data) {
          if constexpr (!IsNotSTL<T>)
            saveSTL(data, format, os, prettyPrint, indent);
          else
            saveNonSTL(data, format, os, prettyPrint, indent);
        } else
          os << "null";
      }

      #define _e_SERIALIZER_FUNCTION_INJECT_START
      #define _e_SERIALIZER_FUNCTION_INJECT_END

      template<typename> static constexpr bool _f = false;

      template<typename T> static void save(const T &, Format, std::ostringstream &, bool, int) {
        static_assert(
          _f<T>, R"(
No save overloads were found for the requested type.
 - if the type is a part of the STL, you will need to convert it to another supported STL type
 - if the type is not a part of the STL, make sure the class contains the ENGINE_SERIALIZE_RECORD macro
)"
        );
      }
  };

  unsigned int Serializer::JsonIndentSize = 2;
}

#endif //SERIALIZER_HPP
