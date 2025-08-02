//
// JsonParser.cpp
// Author: Antoine Bastide
// Date: 07.07.2025
//

#include "Engine/Data/JsonParser.hpp"
#include "Engine/Data/JSON.hpp"
#include "Engine/Macros/Profiling.hpp"

#if ENGINE_DEBUG
#define THROW_JSON_ERROR(msg) throw std::logic_error(std::string(msg) + " at line " + std::to_string(line) + ", column " + std::to_string(column))
#else
#define THROW_JSON_ERROR(msg) throw std::logic_error(msg)
#endif

namespace Engine {
  JSONParser::JSONParser(std::istream &stream)
    : stream(&stream), streamView(""), ptr(nullptr), end(nullptr), pendingKeyIsSet(), commaDetected(true),
      foundData(false), line(1), column(1) {}

  JSONParser::JSONParser(const std::string_view json)
    : stream(nullptr), streamView(json), ptr(nullptr), end(nullptr), pendingKeyIsSet(), commaDetected(true),
      foundData(false), line(1), column(1) {}

  static constexpr bool validNumberChar[256] = {
    ['0'] = true, ['1'] = true, ['2'] = true, ['3'] = true, ['4'] = true,
    ['5'] = true, ['6'] = true, ['7'] = true, ['8'] = true, ['9'] = true,
    ['e'] = true, ['E'] = true, ['-'] = true, ['+'] = true, ['.'] = true
  };

  static constexpr char escapedMap[256] = {
    ['"'] = '"', ['\\'] = '\\', ['/'] = '/', ['b'] = '\b',
    ['f'] = '\f', ['n'] = '\n', ['r'] = '\r', ['t'] = '\t'
  };

  JSON JSONParser::Parse() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSystem);

    JSON json;
    stack.push_back(&json);

    constexpr size_t blockSize = 1024 * 64;
    size_t bufferSize = blockSize;

    std::vector<char> buffer(bufferSize);
    size_t buffered = 0;
    size_t consumed = 0;

    std::istream &iss = stream ? *stream : streamView;
    while (!stack.empty() && (iss || buffered - consumed > 0)) {
      // Grow
      if (const size_t unconsumed = buffered - consumed; unconsumed < bufferSize) {
        std::memmove(buffer.data(), buffer.data() + consumed, unconsumed);
        buffered = unconsumed;
        consumed = 0;
      } else {
        bufferSize *= 2;
        buffer.resize(bufferSize);
        buffer.reserve(bufferSize);
      }

      // Refill if stream is good
      if (iss && buffered < bufferSize) {
        iss.read(buffer.data() + buffered, bufferSize - buffered);
        const size_t readCount = static_cast<size_t>(iss.gcount());
        if (readCount == 0 && iss.eof())
          break;
        buffered += readCount;
      }

      consumed = parseBuffer(buffer.data(), buffered, iss);
    }

    if (!foundData)
      THROW_JSON_ERROR("Empty input is not valid JSON");

    if (!stack.empty()) {
      if (stack.back()->IsObject())
        THROW_JSON_ERROR("Missing closing '}' for object");
      if (stack.back()->IsArray())
        THROW_JSON_ERROR("Missing closing ']' for array");
    }

    if (buffered - consumed > 0 || iss.peek() != EOF) {
      ++column;
      eof(buffer[consumed + 1]);
    }

    return json;
  }

  int JSONParser::parseBuffer(const char *buffer, const size_t buffered, const std::istream &is) {
    const char *p = buffer;
    const char *bufferEnd = buffer + buffered;

    while (p < bufferEnd) {
      #if ENGINE_DEBUG
      ++column;
      #endif

      switch (const char c = *p; tokenTable[static_cast<unsigned char>(c)]) {
        case TOKEN_WHITESPACE:
          break;
        case TOKEN_NEWLINE:
          #if ENGINE_DEBUG
          ++line;
          column = 1;
          #endif
          break;
        case TOKEN_OBJECT_START:
          if (stack.back()->IsObject())
            stack.push_back(setObjectValue(JSON::Object()));
          else if (stack.back()->IsArray()) {
            setArrayValue(JSON::Object());
            stack.push_back(&stack.back()->Back());
          } else if (stack.size() == 1)
            *stack.back() = JSON::Object();
          foundData = true;
          break;
        case TOKEN_OBJECT_END:
          if (stack.back()->IsObject()) {
            if (pendingKeyIsSet)
              THROW_JSON_ERROR("Missing value for key '" + pendingKey + "' in object");
            if (commaDetected)
              THROW_JSON_ERROR("Trailing ',' before closing '}'");
            stack.pop_back();
            if (stack.empty())
              return static_cast<int>(p - buffer + 1);
          } else if (stack.back()->IsArray())
            THROW_JSON_ERROR("Expected ']' but found '}'");
          foundData = true;
          break;
        case TOKEN_ARRAY_START:
          if (stack.back()->IsObject())
            stack.push_back(setObjectValue(JSON::Array()));
          else if (stack.back()->IsArray()) {
            setArrayValue(JSON::Array());
            stack.push_back(&stack.back()->Back());
          } else if (stack.size() == 1)
            *stack.back() = JSON::Array();
          foundData = true;
          break;
        case TOKEN_ARRAY_END:
          if (stack.back()->IsArray()) {
            if (commaDetected)
              THROW_JSON_ERROR("Trailing ',' before closing ']'");
            stack.pop_back();
            if (stack.empty())
              return static_cast<int>(p - buffer + 1);
          } else if (stack.back()->IsObject())
            THROW_JSON_ERROR("Expected '}' but found ']'");
          foundData = true;
          break;
        case TOKEN_COLON:
          if (stack.back()->IsArray())
            THROW_JSON_ERROR("Unexpected ':' in an array, did you mean ','?");
          pendingKey = candidateKey;
          pendingKeyIsSet = true;
          foundData = true;
          candidateKey.clear();
          break;
        case TOKEN_COMMA:
          if (commaDetected)
            THROW_JSON_ERROR("Duplicate ','");
          commaDetected = true;
          pendingKeyIsSet = false;
          pendingKey.clear();
          candidateKey.clear();
          foundData = true;
          break;
        case TOKEN_STRING: {
          ptr = p;
          const char *q = p + 1;
          bool done = false;
          while (q < bufferEnd) {
            if (*q == '"' && q[-1] != '\\') {
              end = q;
              JSON json = parseString();
              foundData = true;

              if (stack.back()->IsObject()) {
                if (!pendingKeyIsSet) {
                  if (candidateKey.empty())
                    candidateKey = json.GetString();
                  else
                    THROW_JSON_ERROR("Missing a colon after a key");
                } else
                  setObjectValue(json);
              } else if (stack.back()->IsArray())
                setArrayValue(json);
              else if (stack.size() == 1 && !stack.back()->isComplexType()) {
                *stack.back() = json;
                stack.pop_back();
                return static_cast<int>(q - buffer + 1);
              }

              p = q;
              done = true;
              break;
            }
            ++q;
          }
          if (!done) {
            if (is.eof())
              THROW_JSON_ERROR("Unterminated string");
            return static_cast<int>(p - buffer);
          }
          break;
        }
        case TOKEN_NUMBER_START: {
          if (stack.back()->IsObject() && !pendingKeyIsSet)
            THROW_JSON_ERROR("Expected a key before adding a number");

          ptr = p;
          const char *q = p + 1;
          bool done = false;
          while (q < bufferEnd) {
            if (!validNumberChar[static_cast<unsigned char>(*q)]) {
              end = q;
              JSON json = parseNumber();
              foundData = true;

              if (stack.back()->IsObject())
                setObjectValue(json);
              else if (stack.back()->IsArray())
                setArrayValue(json);
              else if (stack.size() == 1 && !stack.back()->isComplexType()) {
                *stack.back() = json;
                stack.pop_back();
                return static_cast<int>(q - buffer);
              }

              p = q - 1;
              done = true;
              break;
            }
            ++q;
          }
          if (!done) {
            if (is.eof()) {
              end = q;
              JSON json = parseNumber();
              *stack.back() = json;
              stack.pop_back();
              foundData = true;
              return static_cast<int>(q - buffer);
            }
            return static_cast<int>(p - buffer);
          }
          break;
        }
        case TOKEN_LITERAL_START: {
          if (stack.back()->IsObject() && !pendingKeyIsSet)
            THROW_JSON_ERROR("Expected a key before adding a boolean or null value");

          constexpr std::string_view true_lit = "true";
          constexpr std::string_view false_lit = "false";
          constexpr std::string_view null_lit = "null";

          const std::string_view lit = c == 't' ? true_lit : c == 'f' ? false_lit : null_lit;
          const JSON val = c == 't' ? true : c == 'f' ? false : JSON{};

          if (p + lit.size() > bufferEnd)
            return static_cast<int>(p - buffer);

          for (size_t i = 0; i < lit.size(); ++i)
            if (p[i] != lit[i])
              unexpected(p[i]);

          if (stack.back()->IsObject())
            setObjectValue(val);
          else if (stack.back()->IsArray())
            setArrayValue(val);
          else if (stack.size() == 1 && !stack.back()->isComplexType()) {
            *stack.back() = val;
            foundData = true;
            return static_cast<int>(p - buffer + lit.size());
          }

          p += lit.size() - 1;
          foundData = true;
          break;
        }
        default:
          unexpected(c);
      }

      ++p;
    }

    return static_cast<int>(p - buffer);
  }

  JSON *JSONParser::setObjectValue(const JSON &json) {
    JSON *stackBack = stack.back();

    if (!commaDetected && !stackBack->Empty())
      THROW_JSON_ERROR("Missing ',' between object members");

    if (!pendingKeyIsSet)
      THROW_JSON_ERROR("Expected a key before adding an inner value");

    if (stackBack->Contains(pendingKey))
      THROW_JSON_ERROR(static_cast<std::string>("Duplicate key '") + pendingKey + '\'' + " in object");

    auto &map = stackBack->GetObject();
    auto [it, _] = map.insert_or_assign(pendingKey, json);

    pendingKey.clear();
    candidateKey.clear();
    pendingKeyIsSet = false;
    commaDetected = false;

    return &it->second;
  }

  void JSONParser::setArrayValue(const JSON &json) {
    if (!commaDetected && !stack.back()->Empty())
      THROW_JSON_ERROR("Missing ',' between array members");
    stack.back()->PushBack(json);
    commaDetected = false;
  }

  JSON JSONParser::parseNumber() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);

    // Optional minus
    bool neg = false;
    const char *start = ptr;
    if (*ptr == '-') {
      neg = true;
      ++ptr;
    }

    if (ptr == end)
      THROW_JSON_ERROR("Invalid number: digit expected after '-'");

    uint64_t intPart = 0;
    if (*ptr == '0') {
      ++ptr;
      if (isdigit(*ptr))
        THROW_JSON_ERROR("Invalid number: Leading zeros are not allowed");
    } else if (isdigit(*ptr))
      while (ptr < end && isdigit(*ptr)) {
        intPart = intPart * 10 + (*ptr - '0');
        ++ptr;
      }
    else
      THROW_JSON_ERROR("Invalid number: digit expected after '-'");

    // Optional fraction
    double fracPart = 0.0;
    if (ptr < end && *ptr == '.') {
      ++ptr;
      if (ptr == end || !isdigit(*ptr))
        THROW_JSON_ERROR("Invalid number: digit expected after '.'");
      double factor = 0.1;
      while (ptr < end && isdigit(*ptr)) {
        fracPart += (*ptr - '0') * factor;
        factor *= 0.1;
        ++ptr;
      }
    }
    double value = intPart + fracPart;

    // Optional exponent
    if (ptr < end && (*ptr == 'e' || *ptr == 'E')) {
      ++ptr;
      bool expNeg = false;
      if (ptr < end && (*ptr == '+' || *ptr == '-')) {
        expNeg = *ptr == '-';
        ++ptr;
      }
      if (ptr == end || !isdigit(*ptr))
        THROW_JSON_ERROR("Invalid number: digit expected after exponent");
      int exponent = 0;
      while (ptr < end && isdigit(*ptr)) {
        exponent = exponent * 10 + (*ptr - '0');
        ++ptr;
      }
      value *= std::pow(10.0, exponent * (1 - 2 * expNeg));
    }

    #if ENGINE_DEBUG
    column += ptr - start;
    #endif
    return JSON(value * (1 - 2 * neg));
  }

  std::string result;

  std::string hexString(const uint16_t value) {
    static constexpr char hexDigits[] = "0123456789ABCDEF";
    std::string str;

    str[0] = hexDigits[value >> 12 & 0xF];
    str[1] = hexDigits[value >> 8 & 0xF];
    str[2] = hexDigits[value >> 4 & 0xF];
    str[3] = hexDigits[value & 0xF];

    return str;
  }

  JSON JSONParser::parseString() {
    ENGINE_PROFILE_FUNCTION(Settings::Profiling::ProfilingLevel::PerSubSystem);

    const char *start = ptr;
    if (const size_t newSize = end - ptr - 2; newSize > result.capacity())
      result.reserve(newSize);
    result.clear();

    while (ptr < end) {
      if (const char c = *ptr++; c == '\\') {
        if (ptr == end)
          THROW_JSON_ERROR("Unterminated escape sequence");

        if (const char escaped = escapedMap[*ptr])
          result.push_back(escaped);
        else if (*ptr++ == 'u') {
          if (end - ptr < 4)
            THROW_JSON_ERROR("Incomplete unicode escape");

          const uint16_t first = parseHex4();
          uint32_t code = first;

          if (0xD800 <= first && first <= 0xDBFF) {
            if (end - ptr < 6)
              THROW_JSON_ERROR("Unexpected end of input: missing low surrogate after high surrogate (\\uXXXX)");

            if (ptr[0] != '\\' || ptr[1] != 'u')
              THROW_JSON_ERROR("Expected '\\u' after high surrogate, found '" + std::string(ptr, ptr + 2) + "'");

            ptr += 2;
            const uint16_t second = parseHex4();

            if (second < 0xDC00 || second > 0xDFFF)
              THROW_JSON_ERROR(
              "Invalid low surrogate: expected value in range \\uDC00..\\uDFFF, got \\u" + hexString(second)
            );

            // Combine surrogate pair into full code point
            code = 0x10000 + ((first - 0xD800) << 10 | second - 0xDC00);
          } else if (0xDC00 <= first && first <= 0xDFFF)
            THROW_JSON_ERROR("Unexpected low surrogate \\u" + hexString(first) + " without preceding high surrogate");

          if (code < 0x80)
            result.push_back(static_cast<char>(code));
          else if (code < 0x800) {
            result.push_back(static_cast<char>(0xC0 | code >> 6));
            result.push_back(static_cast<char>(0x80 | code & 0x3F));
          } else {
            result.push_back(static_cast<char>(0xE0 | code >> 12));
            result.push_back(static_cast<char>(0x80 | code >> 6 & 0x3F));
            result.push_back(static_cast<char>(0x80 | code & 0x3F));
          }
          break;
        } else
          THROW_JSON_ERROR("Invalid escape sequence");
      } else {
        const char *runStart = ptr;
        while (ptr < end && *ptr != '"' && *ptr != '\\') {
          if (static_cast<unsigned char>(*ptr) < 0x20)
            THROW_JSON_ERROR("Control character in string");
          ++ptr;
        }
        result.append(runStart, ptr - runStart);
      }
    }

    #if ENGINE_DEBUG
    column += ptr - start;
    #endif
    return JSON(result);
  }

  uint16_t JSONParser::parseHex4() {
    uint16_t code = 0;
    for (int i = 0; i < 4; ++i) {
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

  void JSONParser::unexpected(const char c) const {
    if (static_cast<int>(c) == 0)
      THROW_JSON_ERROR("Unexpected character '\\0'");
    THROW_JSON_ERROR(
      static_cast<std::string>("Unexpected character '") + c + '\''
    );
  }

  void JSONParser::eof(const char c) const {
    if (static_cast<int>(c) != 0)
      THROW_JSON_ERROR(
      static_cast<std::string>("Unexpected character '") + c + "' after JSON end"
    );
  }
}
