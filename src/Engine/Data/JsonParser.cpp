//
// JsonParser.cpp
// Author: Antoine Bastide
// Date: 07.07.2025
//

#include "Engine/Data/JsonParser.hpp"
#include "Engine/Data/JSON.hpp"
#include "Engine/Macros/Profiling.hpp"

#define THROW_JSON_ERROR(msg) throw std::logic_error(std::string(msg) + " at line " + std::to_string(line) + ", column " + std::to_string(column))

namespace Engine::Data {
  JSONParser::JSONParser(const std::string_view json)
    : ptr(json.data()), end(json.data() + json.size()), is(), pendingKeyIsSet(), commaDetected(true), foundData(),
      line(1),
      column() {}

  JSONParser::JSONParser(std::istream *is)
    : ptr(nullptr), end(nullptr), is(is), pendingKeyIsSet(), commaDetected(true), foundData(),
      line(1), column() {}

  static constexpr bool wsTable[256] = {
    [' '] = true, ['\n'] = true, ['\r'] = true, ['\t'] = true
  };

  static constexpr bool validNumEndTable[256] = {
    [' '] = true, ['\n'] = true, ['\r'] = true, ['\t'] = true,
    ['}'] = true, [']'] = true, [','] = true, ['\0'] = true
  };

  JSON JSONParser::Parse() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSystem);
    if (ptr)
      return parseFromString();
    return parseFromIStream();
  }

  JSON JSONParser::parseFromString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSystem);
    skipWhitespace();
    JSON json = parseValueFromString();
    skipWhitespace();

    if (ptr == end)
      return json;
    THROW_JSON_ERROR("Extra data after JSON value");
  }

  JSON JSONParser::parseValueFromString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);
    skipWhitespace();

    switch (*ptr) {
      case '{':
        return parseObjectFromString();
      case '[':
        return parseArrayFromString();
      case '"':
        return parseStringFromString();
      case 't':
        for (int i = 0; i < 4; i++) {
          advance(*ptr);
          i++;
        }
        return JSON(true);
      case 'f':
        for (int i = 0; i < 5; i++) {
          advance(*ptr);
          i++;
        }
        return JSON(false);
      case 'n':
        for (int i = 0; i < 4; i++) {
          advance(*ptr);
          i++;
        }
        return JSON();
      default:
        return parseNumberFromString();
    }
  }

  JSON JSONParser::parseNumberFromString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);

    if (*ptr != '-' && !isdigit(*ptr))
      THROW_JSON_ERROR("Unexpected character '" + std::string(1, *ptr) + "'");

    const char *start = ptr;

    // Optional minus
    if (*ptr == '-') {
      advance(*ptr);
      ++ptr;
    }

    if (ptr == end)
      THROW_JSON_ERROR("Invalid number: digit expected after '-'");

    if (*ptr == '0') {
      advance(*ptr);
      ++ptr;

      if (isdigit(*ptr))
        THROW_JSON_ERROR("Invalid number: Leading zeros are not allowed");
    } else if (isdigit(*ptr))
      while (ptr < end && isdigit(*ptr)) {
        advance(*ptr);
        ++ptr;
      }
    else
      THROW_JSON_ERROR("Invalid number: digit expected after '-'");

    // Optional fraction
    if (ptr < end && *ptr == '.') {
      advance(*ptr);
      ++ptr;
      if (ptr == end || !isdigit(*ptr))
        THROW_JSON_ERROR("Invalid number: digit expected after '.'");
      while (ptr < end && isdigit(*ptr)) {
        advance(*ptr);
        ++ptr;
      }
    }

    // Optional exponent
    if (ptr < end && (*ptr == 'e' || *ptr == 'E')) {
      advance(*ptr);
      ++ptr;
      if (ptr < end && (*ptr == '+' || *ptr == '-')) {
        advance(*ptr);
        ++ptr;
      }
      if (ptr == end || !isdigit(*ptr))
        THROW_JSON_ERROR("Invalid number: digit expected after exponent");
      while (ptr < end && isdigit(*ptr)) {
        advance(*ptr);
        ++ptr;
      }
    }

    if (ptr < end && !validNumEndTable[*ptr])
      unexpected(*ptr);

    return JSON(std::strtod(start, const_cast<char **>(&ptr)));
  }

  JSON JSONParser::parseStringFromString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);

    if (*ptr != '"')
      THROW_JSON_ERROR("Expected '\"' at start of string");

    advance(*ptr);
    ++ptr;
    std::string result;

    while (ptr < end) {
      const char c = *ptr++;
      advance(c);
      if (c == '"')
        return JSON(result);

      if (c == '\\') {
        if (ptr == end)
          THROW_JSON_ERROR("Unterminated escape sequence");

        advance(*ptr);
        switch (*ptr++) {
          case '"':
            result += '"';
            break;
          case '\\':
            result += '\\';
            break;
          case '/':
            result += '/';
            break;
          case 'b':
            result += '\b';
            break;
          case 'f':
            result += '\f';
            break;
          case 'n':
            result += '\n';
            break;
          case 'r':
            result += '\r';
            break;
          case 't':
            result += '\t';
            break;
          case 'u': {
            if (end - ptr < 4)
              THROW_JSON_ERROR("Incomplete unicode escape");

            const uint16_t first = parseHex4();
            uint32_t code = first;

            if (0xD800 <= first && first <= 0xDBFF) {
              if (end - ptr < 6 || ptr[0] != '\\' || ptr[1] != 'u')
                THROW_JSON_ERROR("Invalid Unicode surrogate pair in string");
              ptr += 2;
              const uint16_t second = parseHex4();
              if (second < 0xDC00 || second > 0xDFFF)
                THROW_JSON_ERROR("Invalid Unicode surrogate pair in string");

              code = 0x10000 + ((first - 0xD800) << 10 | second - 0xDC00);
            } else if (0xDC00 <= first && first <= 0xDFFF)
              THROW_JSON_ERROR("Invalid Unicode surrogate pair in string");

            appendUTF8(result, code);
            break;
          }

          default:
            THROW_JSON_ERROR("Invalid escape sequence");
        }
      } else {
        if (static_cast<unsigned char>(c) < 0x20)
          THROW_JSON_ERROR("Control character in string");
        result += c;
      }
    }

    THROW_JSON_ERROR("Unterminated string");
  }

  JSON JSONParser::parseArrayFromString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);
    if (*ptr != '[')
      THROW_JSON_ERROR("Expected '[' at start of array");

    advance(*ptr);
    ++ptr;
    skipWhitespace();
    JSON arr = JSON::Array();

    if (*ptr == ']') {
      advance(*ptr);
      ++ptr;
      return arr;
    }

    while (true) {
      arr.PushBack(parseValueFromString());
      skipWhitespace();

      if (*ptr == ',') {
        advance(*ptr);
        ++ptr;
        skipWhitespace();
        continue;
      }
      if (*ptr == ']') {
        advance(*ptr);
        ++ptr;
        break;
      }
      THROW_JSON_ERROR("Expected ',' or ']' in array");
    }

    return arr;
  }

  JSON JSONParser::parseObjectFromString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);
    if (*ptr != '{')
      THROW_JSON_ERROR("Expected '{' at start of object");

    advance(*ptr);
    ++ptr;
    skipWhitespace();
    JSON obj = JSON::Object();

    if (*ptr == '}') {
      advance(*ptr);
      ++ptr;
      return obj;
    }

    while (true) {
      if (*ptr != '"')
        THROW_JSON_ERROR("Expected string key in object");
      auto key = parseStringFromString().Get<std::string>();

      skipWhitespace();

      // Parse colon
      if (*ptr != ':')
        THROW_JSON_ERROR("Expected ':' after key in object");
      advance(*ptr);
      ++ptr;
      skipWhitespace();

      // Parse value
      obj.Emplace(std::move(key), parseValueFromString());
      skipWhitespace();

      if (*ptr == ',') {
        advance(*ptr);
        ++ptr;
        skipWhitespace();
        continue;
      }
      if (*ptr == '}') {
        advance(*ptr);
        ++ptr;
        break;
      }
      THROW_JSON_ERROR("Expected ',' or '}' in object");
    }

    return obj;
  }

  void JSONParser::skipWhitespace() {
    while (ptr != end && wsTable[static_cast<unsigned char>(*ptr)]) {
      advance(*ptr);
      ++ptr;
    }
  }

  uint16_t JSONParser::parseHex4() {
    uint16_t code = 0;
    for (int i = 0; i < 4; ++i) {
      advance(*ptr);
      const char c = *ptr++;
      code <<= 4;
      if (c >= '0' && c <= '9')
        code |= c - '0';
      else if (c >= 'a' && c <= 'f')
        code |= c - 'a' + 10;
      else if (c >= 'A' && c <= 'F')
        code |= c - 'A' + 10;
      else
        THROW_JSON_ERROR("Invalid hex digit in \\uXXXX");
    }
    return code;
  }

  void JSONParser::appendUTF8(std::string &out, const uint16_t hex4) {
    if (hex4 < 0x80)
      out += static_cast<char>(hex4);
    else if (hex4 < 0x800) {
      out += static_cast<char>(0xC0 | hex4 >> 6);
      out += static_cast<char>(0x80 | hex4 & 0x3F);
    } else {
      out += static_cast<char>(0xE0 | hex4 >> 12);
      out += static_cast<char>(0x80 | hex4 >> 6 & 0x3F);
      out += static_cast<char>(0x80 | hex4 & 0x3F);
    }
  }

  JSON JSONParser::parseFromIStream() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSystem);

    JSON json;
    stack.push_back(&json);

    constexpr size_t blockSize = 1024 * 64;
    size_t bufferSize = blockSize;

    std::vector<char> buffer(bufferSize);
    size_t buffered = 0;
    size_t consumed = 0;

    std::istream &iss = *is;
    while (!stack.empty() && (iss || buffered - consumed > 0)) {
      // Grow
      if (const size_t unconsumed = buffered - consumed; unconsumed < bufferSize) {
        std::memmove(buffer.data(), buffer.data() + consumed, unconsumed);
        buffered = unconsumed;
        consumed = 0;
      } else {
        bufferSize *= 2;
        buffer.resize(bufferSize);
      }

      // Refill if stream is good
      if (iss && buffered < bufferSize) {
        iss.read(buffer.data() + buffered, bufferSize - buffered);
        const size_t readCount = static_cast<size_t>(iss.gcount());
        if (readCount == 0 && iss.eof())
          break;
        buffered += readCount;
      }

      consumed = parseBuffer(buffer.data(), buffered, bufferSize);
    }

    if (!foundData)
      THROW_JSON_ERROR("Empty input is not a valid JSON text");

    if (pendingKeyIsSet)
      THROW_JSON_ERROR("Missing value for key '" + pendingKey + "' in object");

    if (!stack.empty()) {
      if (stack.back()->IsObject())
        THROW_JSON_ERROR("Missing closing '}' for object");
      if (stack.back()->IsArray())
        THROW_JSON_ERROR("Missing closing ']' for array");
    }

    if (commaDetected && ((!stack.empty() &&
                           (stack.back()->IsArray() || stack.back()->IsObject()) && stack.back()->Size() > 1)
                          || ((json.IsArray() || json.IsObject()) && json.Size() > 1)))
      THROW_JSON_ERROR("Unexpected ','; missing value?");

    if (buffered - consumed > 0 || iss.peek() != EOF)
      THROW_JSON_ERROR("Extra data after JSON value");

    return json;
  }

  int JSONParser::parseBuffer(const char *buffer, const size_t bufferSize, const size_t bufferMaxSize) {
    int consumed = 0;
    while (consumed < bufferSize) {
      // Skip whitespaces
      while (consumed < bufferSize && wsTable[static_cast<unsigned char>(buffer[consumed])]) {
        advance(buffer[consumed]);
        ++consumed;
      }

      if (consumed >= bufferSize)
        return consumed;

      // Parse object start
      if (buffer[consumed] == '{') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected '{' after json end");

        if (stack.back()->IsObject()) {
          if (!pendingKeyIsSet)
            THROW_JSON_ERROR("Missing ',' between object members");

          const auto key = pendingKey;
          setObjectValue(JSON::Object());
          stack.push_back(&(*stack.back())[key]);
        } else if (stack.back()->IsArray()) {
          setArrayValue(JSON::Object());
          stack.push_back(&stack.back()->Back());
        } else if (stack.size() == 1)
          *stack.back() = JSON::Object();

        advance(buffer[consumed]);
        foundData = true;
      }
      // Parse object end
      else if (buffer[consumed] == '}') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected '}' outside of object");

        if (const auto obj = stack.back(); obj->IsObject()) {
          if (pendingKeyIsSet)
            THROW_JSON_ERROR("Missing value for key '" + pendingKey + "' in object");
          if (commaDetected && stack.back()->Size() > 1)
            THROW_JSON_ERROR("Trailing ',' before closing '}'");
          stack.pop_back();
          if (stack.empty()) {
            advance(buffer[consumed]);
            return consumed + 1;
          }
        } else if (stack.back()->IsArray())
          THROW_JSON_ERROR("Expected ']' but found '}'");

        advance(buffer[consumed]);
        foundData = true;
      }
      // Parse array start
      else if (buffer[consumed] == '[') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected '[' after json end");
        if (stack.back()->IsObject()) {
          if (!pendingKeyIsSet)
            THROW_JSON_ERROR("Expected a key before adding an inner array");

          const auto key = pendingKey;
          setObjectValue(JSON::Array());
          stack.push_back(&(*stack.back())[key]);
        } else if (stack.back()->IsArray()) {
          setArrayValue(JSON::Array());
          stack.push_back(&stack.back()->Back());
        } else if (stack.size() == 1)
          *stack.back() = JSON::Array();

        advance(buffer[consumed]);
        foundData = true;
      }
      // Parse array end
      else if (buffer[consumed] == ']') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected ']' outside of object");
        if (const auto arr = stack.back(); arr->IsArray()) {
          if (commaDetected && stack.back()->Size() > 1)
            THROW_JSON_ERROR("Trailing ',' before closing ']'");
          stack.pop_back();
          if (stack.empty()) {
            advance(buffer[consumed]);
            return consumed + 1;
          }
        } else if (stack.back()->IsObject())
          THROW_JSON_ERROR("Expected '}' but found ']'");

        advance(buffer[consumed]);
        foundData = true;
      }
      // Parse colon for key detection
      else if (buffer[consumed] == ':') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected ':' after json end");

        if (stack.back()->IsArray())
          THROW_JSON_ERROR("Unexpected ':' in an array, did you mean ','?");

        pendingKey = candidateKey;
        pendingKeyIsSet = true;
        candidateKey.clear();
        advance(buffer[consumed]);
        foundData = true;
      }
      // Parse next element
      else if (buffer[consumed] == ',') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected ',' after json end");

        if (commaDetected)
          THROW_JSON_ERROR("Unexpected ','; missing value?");

        commaDetected = true;
        candidateKey.clear();
        pendingKey.clear();
        pendingKeyIsSet = false;
        foundData = true;
        advance(buffer[consumed]);
      }
      // Parse strings
      else if (buffer[consumed] == '"') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected '\"' after json end");

        ptr = buffer + consumed;
        size_t current = consumed + 1;
        bool foundString = false;

        while (current < bufferSize) {
          if (buffer[current] == '"' && buffer[current - 1] != '\\') {
            end = buffer + current + 1;
            JSON json = parseStringFromString();

            if (stack.back()->IsObject()) {
              if (!pendingKeyIsSet) {
                if (candidateKey.empty())
                  candidateKey = json.Get<std::string>();
                else
                  THROW_JSON_ERROR("Missing a colon after a key");
              } else
                setObjectValue(json);
            } else if (stack.back()->IsArray())
              setArrayValue(json);
            else if (stack.size() == 1)
              *stack.back() = json;

            consumed = current;
            foundString = true;
            foundData = true;
            break;
          }

          current++;
        }

        if (!foundString) {
          if (is->eof()) {
            for (int i = consumed; i < current; i++)
              advance(buffer[i]);
            THROW_JSON_ERROR("Unterminated string");
          }
          break;
        }
      }
      // Parse numbers
      else if (buffer[consumed] == '-' || std::isdigit(buffer[consumed])) {
        if (stack.empty())
          THROW_JSON_ERROR(static_cast<std::string>("Unexpected '") + buffer[consumed] + "' after json end");

        if (stack.back()->IsObject() && !pendingKeyIsSet)
          THROW_JSON_ERROR("Expected a key before adding a number");

        ptr = buffer + consumed;
        size_t current = consumed + 1;
        bool foundNumber = false;

        while (current <= bufferSize) {
          if (validNumEndTable[buffer[current]]) {
            end = buffer + current;
            JSON json = parseNumberFromString();

            if (stack.back()->IsObject()) {
              if (!pendingKeyIsSet)
                candidateKey = json.Get<double>();
              else
                setObjectValue(json);
            } else if (stack.back()->IsArray())
              setArrayValue(json);
            else if (stack.size() == 1)
              *stack.back() = json;

            consumed = current - 1;
            foundNumber = true;
            foundData = true;
            break;
          }

          current++;
        }

        if (!foundNumber) {
          if (is->eof()) {
            if (stack.back()->IsObject())
              THROW_JSON_ERROR("Missing closing '}' for object");
            if (stack.back()->IsArray())
              THROW_JSON_ERROR("Missing closing ']' for array");

            end = buffer + current;
            JSON json = parseNumberFromString();

            if (stack.back()->IsObject()) {
              if (!pendingKeyIsSet)
                candidateKey = json.Get<double>();
              else
                setObjectValue(json);
            } else if (stack.back()->IsArray())
              setArrayValue(json);
            else if (stack.size() == 1)
              *stack.back() = json;

            consumed = current - 1;
          }

          break;
        }
      }
      // Parse true value
      else if (buffer[consumed] == 't') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected 't' after json end");

        if (stack.back()->IsObject() && !pendingKeyIsSet)
          THROW_JSON_ERROR("Expected a key before adding a boolean");

        const int value = parseBoolOrNullFromIStream(buffer, bufferMaxSize, "true", 4, true, consumed);
        if (value == -1)
          break;
        if (value == 0) {
          for (int i = consumed; i < consumed + 4; i++)
            advance(buffer[consumed]);
          return consumed + 4;
        }
      }
      // Parse false value
      else if (buffer[consumed] == 'f') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected 'f' after json end");

        if (stack.back()->IsObject() && !pendingKeyIsSet)
          THROW_JSON_ERROR("Expected a key before adding a boolean");

        const int value = parseBoolOrNullFromIStream(buffer, bufferMaxSize, "false", 5, false, consumed);
        if (value == -1)
          break;
        if (value == 0) {
          for (int i = consumed; i < consumed + 5; i++)
            advance(buffer[consumed]);
          return consumed + 5;
        }
      }
      // Parse null value
      else if (buffer[consumed] == 'n') {
        if (stack.empty())
          THROW_JSON_ERROR("Unexpected 'n' after json end");

        if (stack.back()->IsObject() && !pendingKeyIsSet)
          THROW_JSON_ERROR("Expected a key before adding a null");

        const int value = parseBoolOrNullFromIStream(buffer, bufferMaxSize, "null", 4, JSON{}, consumed);
        if (value == -1)
          break;
        if (value == 0) {
          for (int i = consumed; i < consumed + 4; i++)
            advance(buffer[consumed]);
          return consumed + 4;
        }
      } else
        unexpected(buffer[consumed]);

      consumed++;
    }

    return std::min(static_cast<int>(bufferSize), consumed);
  }

  int JSONParser::parseBoolOrNullFromIStream(
    const char *buffer, const size_t bufferSize, const char *str, const size_t size, const JSON &value, int &consumed
  ) {
    if (consumed + size > bufferSize)
      return -1;

    advance(buffer[consumed]);

    for (int i = 0; i < size; i++)
      if (buffer[consumed + i] != str[i])
        unexpected(buffer[consumed + i]);

    for (int i = 1; i < size; i++)
      advance(buffer[consumed + i]);

    if (stack.back()->IsObject()) {
      if (!pendingKeyIsSet)
        THROW_JSON_ERROR("Expected key before value");

      setObjectValue(value);
    } else if (stack.back()->IsArray())
      setArrayValue(value);
    else if (stack.size() == 1) {
      *stack.back() = value;
      foundData = true;
      return 0;
    }

    consumed += size - 1;
    foundData = true;

    return 1;
  }

  void JSONParser::setObjectValue(const JSON &json) {
    if (!commaDetected && !stack.back()->Empty())
      THROW_JSON_ERROR("Missing ',' between object members");

    if (stack.back()->Contains(pendingKey))
      THROW_JSON_ERROR(static_cast<std::string>("Duplicate key '") + pendingKey + '\'' + " in object");

    (*stack.back())[pendingKey] = json;
    pendingKey.clear();
    pendingKeyIsSet = false;
    candidateKey.clear();
    commaDetected = false;
  }

  void JSONParser::setArrayValue(const JSON &json) {
    if (!commaDetected && !stack.back()->Empty())
      THROW_JSON_ERROR("Missing ',' between array members");
    stack.back()->PushBack(json);
    commaDetected = false;
  }

  void JSONParser::advance(const char c) {
    if (c == '\n') {
      line++;
      column = 1;
    } else
      column++;
  }

  void JSONParser::unexpected(const char c) const {
    if (static_cast<int>(c) == 0)
      THROW_JSON_ERROR("Unexpected character '\\0'");
    THROW_JSON_ERROR(
      static_cast<std::string>("Unexpected character '") + c + '\''
    );
  }
}
