//
// Game2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>
#include <cmrc/cmrc.hpp>

#include "Common/PrimitiveTypes.h"
#include "Input/Keyboard.h"
#include "Rendering/SpriteRenderer.h"

// TODO: Make resource embedding an option

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Engine2D {
  class Entity2D;
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Entity2D;
    friend class ResourceManager;
    public:
      [[nodiscard]] static int16 Width();
      [[nodiscard]] static int16 Height();
      [[nodiscard]] static float DeltaTime();

      /**
       * Start's and Run's the current game
       * @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
       */
      void Run();
      /**
       * Set's the given resource loader of the game to load embedded resources
       * @param resourceLoader The resource loader that contains the resources to load
       * @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
       */
      void SetGameResourceLoader(const ResourceLoader &resourceLoader);

      static void Close(Engine::Input::KeyboardContext context);

      /** @returns A pointer to currently running game */
      static Game2D *Instance();
    protected:
      // The width of the game window
      int16 width;
      // The height of the game window
      int16 height;

      /**
       * Creates a game
       * @param width The width of the game window
       * @param height The height of the game window
       * @param title The title of the game window
       */
      Game2D(int16 width, int16 height, std::string title);
      virtual ~Game2D() = default;

      /** Called during initialization, allowing derived classes to customize behavior. */
      virtual void Initialize() {}
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      virtual void Update() {}
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      virtual void Quit() {}
    private:
      // The title of the game window
      std::string title;
      // The pointer to the game window
      GLFWwindow *window;
      // The current game instance, unique
      static Game2D *instance;
      // The resource loader for embedded resources
      ResourceLoader resourceLoader;

      // The time during two frames
      float deltaTime;
      // The root entity of the game
      // TODO: Replace this with a scene object that has it's own root and list of entities
      Entity2D *root;
      // All the entities currently in the game
      std::vector<Entity2D *> entities;
      // All the entities currently in the game
      std::unordered_set<Entity2D *> entitiesSearch;
      // The sprite renderer used to render all the textures of the game
      Rendering::SpriteRenderer *spriteRenderer;

      float frameRate;

      /** Initializes all elements of the game */
      void initialize();
      /** Update's all elements of the game */
      void update();
      /** Render's all elements of the game */
      void render() const;
      /** Clean's up the game's variables and stops it from running any longer */
      void quit();

      /** Loads the given resource from the embedded resources */
      [[nodiscard]] cmrc::file loadResource(const std::string &path) const;

      /** Adds the given entity to the game so that it can be managed i.e. initialized, updated, rendered and quit */
      static void AddEntity(Entity2D *entity);
      /** Removes the given entity from the game so that it will no longer be managed i.e. initialized, updated, rendered and quit */
      static void RemoveEntity(Entity2D *entity);

      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
  };
} // Engine2D

#endif //GAME2D_H
