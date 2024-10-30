//
// Game2D.h
// Game2D: 
// Author: Antoine Bastide
// Date: 30/10/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include "../Common/Headers/RenderingHeaders.h"

namespace Engine2D {
  class Game2D {
    public:
      Game2D(int width, int height, char* title);
      int Initialize();
      void Update(float deltaTime);
      void ProcessInput() const;
      void Render();
      void Quit();
    private:
      int width, height;
      char* title;
      GLFWwindow* window;
      ~Game2D();
  };
}


#endif //GAME2D_H
