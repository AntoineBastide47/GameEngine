//
// Game2D.h
// Game2D: 
// Author: Antoine Bastide
// Date: 30/10/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <string>

#include "../Common/Headers/RenderingHeaders.h"

namespace Engine2D {
  class Game2D {
    public:
      Game2D(int width, int height, const std::string &title);
      ~Game2D();
      int Initialize();
      void Update(float deltaTime) const;
      void ProcessInput() const;
      void Render() const;
      void Quit();
      [[nodiscard]] bool IsRunning() const;
    private:
      int width, height;
      const char* title;
      GLFWwindow* window = nullptr;
  };
}


#endif //GAME2D_H
