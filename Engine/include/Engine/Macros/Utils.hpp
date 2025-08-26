//
// Utils.h
// Author: Antoine Bastide
// Date: 08/03/2025
//

#ifndef UTILS_H
#define UTILS_H

#include <string_view>

#define SAFE_DELETE(p) if (p) { delete p; p = nullptr; }
#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x

#ifdef _MSC_VER
#define ENGINE_FUNC_SIG __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define ENGINE_FUNC_SIG __PRETTY_FUNCTION__
#else
#error "Unknown Compiler"
#endif

constexpr std::string_view className(const std::string_view prettyFunction, const bool fullyQualified) {
  const size_t colons = prettyFunction.rfind("::");
  if (colons == std::string_view::npos)
    return "::";

  if (!fullyQualified) {
    const size_t end = colons;
    size_t start = end;
    int angle_depth = 0;

    while (start > 0) {
      --start;
      if (const char c = prettyFunction[start]; c == '>')
        angle_depth++;
      else if (c == '<')
        angle_depth--;
      else if (angle_depth == 0 && (c == ':' || c == ' '))
        break;
    }
    ++start;
    return prettyFunction.substr(start, end - start);
  }

  const size_t end = colons;
  size_t start = end;
  int angle_depth = 0;

  while (start > 0) {
    --start;
    if (const char c = prettyFunction[start]; c == '>')
      angle_depth++;
    else if (c == '<')
      angle_depth--;
    else if (angle_depth == 0 &&
             !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == ':'))
      break;
  }
  ++start;
  return prettyFunction.substr(start, end - start);
}

#define ENGINE_CLASS_NAME className(ENGINE_FUNC_SIG, false)
#define ENGINE_CLASS_NAME_FULLY_QUALIFIED className(ENGINE_FUNC_SIG, true)

#endif //UTILS_H
