//
// Utils.h
// Author: Antoine Bastide
// Date: 08/03/2025
//

#ifndef UTILS_H
#define UTILS_H

#define SAFE_DELETE(p) if (p) { delete p; p = nullptr; }
#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x

#ifdef _MSC_VER
#define ENGINE_FUNC_SIG __FUNCSIG__
#else
#define ENGINE_FUNC_SIG __PRETTY_FUNCTION__
#endif

#endif //UTILS_H
