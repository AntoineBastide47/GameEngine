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

#include "Input/InputContexts.h"
#include "Rendering/SpriteRenderer.h"

// TODO: Make resource embedding an option

using ResourceLoader = std::function<cmrc::file(const std::string &)>;
namespace Engine2D {
  class Entity2D;
  class Transform2D;
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Entity2D;
    friend class ResourceManager;
    friend class Transform2D;
    public:
      [[nodiscard]] static int Width();
      [[nodiscard]] static int Height();
      [[nodiscard]] static int InitialWidth();
      [[nodiscard]] static int InitialHeight();
      [[nodiscard]] static Vector2 AspectRatio();


      [[nodiscard]] static float DeltaTime();
      [[nodiscard]] static float FixedDeltaTime();

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

      static void Close(Engine::Input::KeyboardAndMouseContext context);

      /** @returns A pointer to currently running game */
      static Game2D *Instance();
    protected:
      // The width of the game window
      int width;
      // The height of the game window
      int height;

      /**
       * Creates a game
       * @param width The width of the game window
       * @param height The height of the game window
       * @param title The title of the game window
       */
      Game2D(int width, int height, std::string title);
      virtual ~Game2D() = default;

      /** Called during initialization, allowing derived classes to customize behavior. */
      virtual void Initialize() {}
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      virtual void Update() {}
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      virtual void Quit() {}
    private:
      // The initial width of the game window
      int initialWidth;
      // The initial height of the game window
      int initialHeight;
      // The current aspect ratio of the window
      Vector2 aspectRatio;

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
      // The time during two frames
      // The root entity of the game
      // TODO: Replace this with a scene object that has it's own root and list of entities
      Entity2D *root;
      // All the entities currently in the game
      std::vector<Entity2D *> entities;
      // All the entities currently in the game
      std::unordered_set<Entity2D *> entitiesSearch;
      // The sprite renderer used to render all the textures of the game
      Rendering::SpriteRenderer *spriteRenderer;
      Rendering::ShapeRenderer *shapeRenderer;
      Physics::Physics2D *physics2D;

      float frameRate;
      float lastKAndMInput = 0.0f;
      float lastGamepadInput = 0.0f;
      float physicsTimeStep = 0.02f;

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
      static void AddEntity(Entity2D &entity);
      /** Removes the given entity from the game so that it will no longer be managed i.e. initialized, updated, rendered and quit */
      static void RemoveEntity(Entity2D &entity);

      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
  };
} // Engine2D

#endif //GAME2D_H
