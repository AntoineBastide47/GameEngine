//
// JsonParser.hpp
// Author: Antoine Bastide
// Date: 07.07.2025
//

#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <iosfwd>
#include <string_view>
#include <vector>

namespace Engine::Data {
  class JSON;

  class JSONParser final {
    public:
      /// Creates a parser from the given std::string_view
      explicit JSONParser(std::string_view json);
      /// Creates a parser from the given std::istream
      explicit JSONParser(std::istream *is);

      /// @returns The parsed data stored in the parser
      JSON Parse();
    private:
      /// The current character in the string_view being parsed
      const char *ptr;
      /// The last character in the string_view being parsed
      const char *end;

      /// Parses the whole string_view
      JSON parseFromString();
      /// Parses the current value in the string_view
      JSON parseValueFromString();
      /// Parses the current number in the string_view
      JSON parseNumberFromString();
      /// Parses the current string in the string_view
      JSON parseStringFromString();
      /// Parses the current array in the string_view
      JSON parseArrayFromString();
      /// Parses the current object in the string_view
      JSON parseObjectFromString();

      /// Skips all the whitespace chars until a non whitespace char is found
      void skipWhitespace();
      /// Converts a hex of size 4 into a uint16_t
      uint16_t parseHex4();
      /// Adds the given hex4 to the output string
      static void appendUTF8(std::string &out, uint16_t hex4);

      /// The current istream being parsed
      std::istream *is;
      /// The current JSON object/array tree
      std::vector<JSON *> stack;
      /// The key waiting to be assigned
      std::string pendingKey;
      /// The string value that could be turned into a key
      std::string candidateKey;
      /// Whether the current pending key has been assigned
      bool pendingKeyIsSet;
      /// Whether a comma has been detected by the parser
      bool commaDetected;
      /// Whether JSON data has been detected in the istream
      bool foundData;

      /// The current line number the parsed character is located on
      size_t line;
      /// The current column number the parsed character is located on
      size_t column;

      /// Parses the whole istream
      JSON parseFromIStream();
      /// Parses the given buffer
      int parseBuffer(const char *buffer, size_t bufferSize, size_t bufferMaxSize);
      /// Parses a boolean or null JSON value
      int parseBoolOrNullFromIStream(
        const char *buffer, size_t bufferSize, const char *str, size_t size, const JSON &value, int &consumed
      );
      /// Assigns the given JSON value to the object at the back of the JSON stack and to the pending key
      void setObjectValue(const JSON &json);
      /// Assigns the given JSON value to the array at the back of the JSON
      void setArrayValue(const JSON &json);

      /// Advances the current char for line/column detection
      void advance(char c);
      /// Throws an unexpected error for the given char
      void unexpected(char c) const;
  };
}

#endif //JSON_PARSER_HPP
