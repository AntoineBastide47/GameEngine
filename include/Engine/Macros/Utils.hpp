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

#endif //UTILS_H
