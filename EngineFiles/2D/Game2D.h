//
// Game2D.h
// Game2D: 
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <string>
#include <vector>

#include "Common/RenderingHeaders.h"
#include "Rendering/SpriteRenderer.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  class Game2D {
    friend class Entity2D;
    public:
      Game2D(int width, int height, std::string title);
      virtual ~Game2D() = default;

      void Run();
      static Game2D *Instance();
    protected:
      Rendering::SpriteRenderer *spriteRenderer;
    private:
      int width, height;
      std::string title;
      GLFWwindow *window;
      static Game2D *instance;
      glm::vec2 s = glm::vec2(0, 0);

      float deltaTime;
      bool keys[1024]{};
      bool processedKeys[1024]{};
      Entity2D *mainParent;
      std::vector<Entity2D *> entities;

      void initialize();
      void processInput() const;
      void update() const;
      void render() const;
      void quit();

      static void AddEntity(Entity2D *entity);
      static void RemoveEntity(Entity2D *entity);

      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
  };
} // Engine2D

#endif //GAME2D_H
