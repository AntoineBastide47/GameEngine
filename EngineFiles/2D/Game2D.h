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

#include "2D/Physics/Physics2D.h"
#include "Common/RenderingHeaders.h"
#include "Input/InputContexts.h"
#include "Types/Vector2.h"

// TODO: Make resource embedding an option

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
    friend class Physics::Physics2D;
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

      /// @returns The width of the viewport of the window
      [[nodiscard]] static float ViewportWidth();
      /// @returns The height of the viewport of the window
      [[nodiscard]] static float ViewportHeight();
      /// @returns The aspect ratio of the viewport
      [[nodiscard]] static Vector2 AspectRatio();

      /// @returns The time span between the current frame and the last frame
      [[nodiscard]] static float DeltaTime();
      /// @returns The fixed time span between the physics updates
      [[nodiscard]] static float FixedDeltaTime();

      /// Closes/Quits the game
      static void Close(Engine::Input::KeyboardAndMouseContext context);
    protected:
      /**
       * Creates a game
       * @param width The width of the game window
       * @param height The height of the game window
       * @param title The title of the game window
       */
      Game2D(int width, int height, std::string title);
      virtual ~Game2D() = default;

      /// Called during initialization, allowing derived classes to customize behavior.
      virtual void Initialize() {}
    private:
      static const float screenScaleFactor;
      /// The initial width of the game window
      int initialWidth;
      /// The initial height of the game window
      int initialHeight;
      /// The current aspect ratio of the window
      Vector2 aspectRatio;

      /// The title of the game window
      std::string title;
      /// The width of the game window
      int width;
      /// The height of the game window
      int height;
      /// The pointer to the game window
      GLFWwindow *window;

      /// The current game instance, unique
      static Game2D *instance;
      /// The resource loader for embedded resources
      ResourceLoader resourceLoader;

      /// TODO: Replace this with a scene object that has it's own root and list of entities
      /// The root entity of the game
      Entity2D *root;
      /// All the entities currently in the game
      std::vector<Entity2D *> entities;
      /// Entities scheduled to be added to the game
      std::vector<Entity2D *> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::vector<Entity2D *> entitiesToRemove;
      /// The entities of the game grouped by there texture id
      std::map<int, std::vector<Entity2D *>> entitiesById;

      /// The time during two frames
      float deltaTime;
      /// The time scaling factor
      float timeScale;
      /// The frame rate at which we want the game to run
      int targetFrameRate;
      /// The rate at which the game should update
      float targetRenderRate;

      /// Physics simulator
      Physics::Physics2D *physics2D;
      /// The time since the last physics simulated step
      float fixedDeltaTime;
      /// Timer used to check if the game is ready for the next physics update
      float physicsAccumulator;

      /// Initializes the game
      void initialize();
      /// Processes all the inputs to the game
      static void processInput();
      /// Update the game
      void update() const;
      /// Simulates a step of the physics simulation
      void fixedUpdate();
      /// Renders the game
      void render() const;
      /// Quits the game
      void quit();

      /// Loads the given resource from the embedded resources
      [[nodiscard]] cmrc::file loadResource(const std::string &path) const;

      /// Adds all the entity scheduled for adding
      void addEntities();
      /// Adds an entity to the game
      static void addEntity(Entity2D *entity);
      /// Removes all the entity scheduled for removal
      void removeEntities();
      /// Removes an entity from the game
      static void removeEntity(Entity2D *entity);

      // OpenGL callbacks
      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void window_refresh_callback(GLFWwindow *window);
      static void window_pos_callback(GLFWwindow *window, int x, int y);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
  };
} // Engine2D

#endif //GAME2D_H
