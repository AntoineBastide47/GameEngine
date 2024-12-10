//
// Game2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <functional>
#include <string>
#include <vector>
#include <cmrc/cmrc.hpp>

#include "Common/RenderingHeaders.h"
#include "Input/InputContexts.h"
#include "Physics/Physics2D.h"

// TODO: Make resource embedding an option
// TODO: Add check files for inline-able functions and test refactor for performance gain

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Engine2D::Physics {
  class Collider2D;
}

namespace Engine2D {
  class Entity2D;
  class Transform2D;
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Entity2D;
    friend class ResourceManager;
    friend class Transform2D;
    friend class Physics::Collider2D;
    public:
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

      /** @returns The width of the viewport of the window */
      [[nodiscard]] static float ViewportWidth();
      /** @returns The height of the viewport of the window */
      [[nodiscard]] static float ViewportHeight();
      /** @returns The aspect ratio of the viewport */
      [[nodiscard]] static Vector2 AspectRatio();

      /** @returns The time span between the current frame and the last frame */
      [[nodiscard]] static float DeltaTime();
      /** @returns The fixed time span between the physics updates */
      [[nodiscard]] static float FixedDeltaTime();

      /** Closes/Quits the game  */
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
      static const float screenScaleFactor;
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
      // TODO: Replace this with a scene object that has it's own root and list of entities
      // The root entity of the game
      Entity2D *root;
      // All the entities currently in the game
      std::vector<Entity2D *> entities;
      // Physics simulator
      Physics::Physics2D *physics2D;

      std::vector<Entity2D *> entitiesToAdd;
      std::vector<Entity2D *> entitiesToRemove;

      float frameRate;
      float renderRate;

      float physicsAccumulator;
      int physicsStepCount;
      float physicsTimeStep = 0.02f;
      int8_t maxPhysicsStepsPerSecond = static_cast<int8_t>(std::clamp(
        static_cast<int>(std::round(1 / physicsTimeStep)), 50, 60
      ));

      /** Initializes all elements of the game */
      void initialize();
      /** Update's all elements of the game */
      void update();
      /** Update's all physics elements of the game */
      void fixedUpdate();
      /** Render's all elements of the game */
      void render() const;
      /** Clean's up the game's variables and stops it from running any longer */
      void quit();

      /** Loads the given resource from the embedded resources */
      [[nodiscard]] cmrc::file loadResource(const std::string &path) const;

      /** Adds all the entities that are pending to be added to the game */
      void AddEntities();
      /** Adds the given entity to the game so that it can be managed i.e. initialized, updated, rendered and quit */
      static void AddEntity(Entity2D *entity);
      /** Removes all the entities that are pending to be removed to the game */
      void RemoveEntities();
      /** Removes the given entity from the game so that it will no longer be managed i.e. initialized, updated, rendered and quit */
      static void RemoveEntity(Entity2D *entity);

      // OpenGL callbacks
      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void window_refresh_callback(GLFWwindow *window);
      static void window_pos_callback(GLFWwindow *window, int x, int y);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
  };
} // Engine2D

#endif //GAME2D_H
