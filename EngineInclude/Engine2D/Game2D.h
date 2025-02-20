//
// Game2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <thread>
#include <functional>
#include <string>
#include <cmrc/cmrc.hpp>

#include "Engine/RenderingHeaders.h"
#include "Engine2D/Entity2D.h"
#include "Engine/Input/InputContexts.h"

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Engine {
  class Settings;
}

namespace Engine2D {
  namespace Rendering {
    class Texture2D;
  }
  namespace Physics {
    class Collider2D;
    class Physics2D;
  }
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Entity2D;
    friend class ResourceManager;
    friend class Transform2D;
    friend class Physics::Collider2D;
    friend class Physics::Physics2D;
    friend class Engine::Settings;
    friend class Rendering::Texture2D;
    friend class ParticleSystemRenderer2D;
    public:
      /// @returns The width of the viewport of the window
      [[nodiscard]] static float ViewportWidth();
      /// @returns The height of the viewport of the window
      [[nodiscard]] static float ViewportHeight();
      /// @returns The aspect ratio of the viewport
      [[nodiscard]] static glm::vec2 AspectRatio();
      /// @returns The time span between the current frame and the last frame
      [[nodiscard]] static float DeltaTime();
      /// @returns The fixed time span between the physics updates
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
      void SetGameResourceLoader(ResourceLoader resourceLoader);

      /// Closes/Quits the game
      static void Close(Engine::Input::KeyboardAndMouseContext context);
      /// Creates an entity of with the given name
      static std::shared_ptr<Entity2D> AddEntity(std::string name = "Entity");
      /// @returns The entity with the given name if it was found, nullptr if not
      static std::shared_ptr<Entity2D> Find(const std::string &name);
    protected:
      /**
       * Creates a game
       * @param width The width of the game window
       * @param height The height of the game window
       * @param title The title of the game window
       */
      Game2D(int width, int height, const char *title);
      virtual ~Game2D() = default;

      /// Called during initialization, allowing derived classes to customize behavior.
      virtual void OnInitialize() {}
    private:
      /// Used to resize the coordinate space. If initial window is 1920x1080 and factor is 0.1f, the coordinate space will
      /// be 192x108.
      static const float screenScaleFactor;
      /// The current aspect ratio of the window
      glm::vec2 aspectRatio;

      /// The title of the game window
      const char *title;
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

      /// All the entities currently in the game
      std::vector<std::shared_ptr<Entity2D>> entities;
      /// Entities scheduled to be added to the game
      std::vector<std::shared_ptr<Entity2D>> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::vector<std::shared_ptr<Entity2D>> entitiesToRemove;
      /// The entities of the game grouped by there texture id
      std::map<int, std::vector<std::shared_ptr<Entity2D>>> entitiesToRender;

      /// The time during two frames
      float deltaTime;
      /// The time scaling factor
      float timeScale;
      /// The frame rate targeted by the engine
      float targetFrameRate;
      /// The rate at which the game should update
      float targetRenderRate;
      /// Counts the number of frames renderer during the current second of elapsed time
      int frameCounter;
      /// The remaining time until the next frame should be rendered
      std::chrono::time_point<std::chrono::steady_clock> nextFrameTime;

      /// Physics simulator
      Physics::Physics2D *physics2D;
      /// Timer used to check if the game is ready for the next physics update
      float physicsAccumulator;

      /// Whether the main thread is finished and ready to sync with the render thread
      bool updateFinished;
      /// Whether the render thread is finished and ready to sync with the main thread
      bool renderFinished;
      std::mutex syncMutex;
      std::condition_variable cv;
      std::thread renderThread;

      void updateLoop();
      void renderLoop();

      /// Initializes the game
      void initialize();
      /// Processes all the inputs to the game
      void processInput();
      /// Update the game
      void update();
      /// Simulates a step of the physics simulation
      void fixedUpdate();
      /// Renders the game
      void render();
      /// Limits the frame rate of the game if needed
      void limitFrameRate();
      /// Quits the game
      void quit();

      /// Loads the given resource from the embedded resources
      [[nodiscard]] cmrc::file loadResource(const std::string &path) const;

      /// Adds all the entity scheduled for adding
      void addEntities();
      /// Removes all the entity scheduled for removal
      void removeEntities();
      /// Removes an entity from the game
      static void removeEntity(const std::shared_ptr<Entity2D> &entity);

      // OpenGL callbacks
      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
  };
} // Engine2D

#endif //GAME2D_H
