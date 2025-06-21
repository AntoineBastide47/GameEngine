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
#else
#define ENGINE_FUNC_SIG __PRETTY_FUNCTION__
#endif

constexpr inline bool is_ident_char(const char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == ':';
}

constexpr inline std::string_view className(std::string_view prettyFunction) {
  const size_t colons = prettyFunction.rfind("::");
  if (colons == std::string_view::npos)
    return "::";

  const size_t end = colons;
  size_t start = end;
  int angle_depth = 0;

  while (start > 0) {
    --start;
    if (const char c = prettyFunction[start]; c == '>')
      angle_depth++;
    else if (c == '<')
      angle_depth--;
    else if (angle_depth == 0 && !is_ident_char(c))
      break;
  }
  ++start;
  return prettyFunction.substr(start, end - start);
}

#define ENGINE_CLASS_NAME className(ENGINE_FUNC_SIG)

#endif //UTILS_H
