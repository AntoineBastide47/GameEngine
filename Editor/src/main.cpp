//
// main.cpp
// Author: Antoine Bastide
// Date: 02.08.2025
//

#include <iostream>

#include "Window.hpp"

int main(int, const char *[]) {
  try {
    Editor::Window{}.Run();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Application error: " << e.what() << std::endl;
    return 1;
  }
}
