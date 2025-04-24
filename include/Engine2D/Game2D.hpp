//
// Game2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <thread>
#include <cmrc/cmrc.hpp>

#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine/Input/InputContexts.hpp"

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Engine {
  class ResourceManager;
  class Settings;
}

namespace Engine2D {
  namespace Rendering {
    class Camera2D;
  }
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Entity2D;
    friend class Engine::ResourceManager;
    friend class Engine::Settings;
    public:
      /// @returns True if the Game2D instance has been initialized, False if not
      [[nodiscard]] static bool Initialized();
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
      /// @returns A reference to the main camera of the game
      [[nodiscard]] static std::shared_ptr<Rendering::Camera2D> MainCamera();

      /**
       * Set's the given resource loader of the game to load embedded resources
       * @param resourceLoader The resource loader that contains the resources to load
       * @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
       */
      void SetGameResourceLoader(ResourceLoader resourceLoader);

      /// Closes/Quits the game
      static void Close(Engine::Input::KeyboardAndMouseContext context);
      /// Creates an entity of with the given name
      static std::shared_ptr<Entity2D> AddEntity(
        std::string name = "Entity", bool isStatic = false, glm::vec2 position = glm::vec2(0.0f, 0.0f),
        float rotation = 0.0f, glm::vec2 scale = glm::vec2(1.0f, 1.0f), std::shared_ptr<Entity2D> parent = nullptr
      );
      /// @returns The entity with the given name if it was found, nullptr if not
      static std::shared_ptr<Entity2D> Find(const std::string &name);
      /**
       * Start's and Run's the current game
       * @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
       */
      void Run();
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
      /// Used to resize the coordinate space. If the initial window is 1920x1080 and factor is 0.1f, the coordinate space will
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
      std::shared_ptr<Rendering::Camera2D> cameraComponent;
      /// The resource loader for embedded resources
      ResourceLoader resourceLoader;

      /// All the entities currently in the game
      std::vector<std::shared_ptr<Entity2D>> entities;
      /// Entities scheduled to be added to the game
      std::vector<std::shared_ptr<Entity2D>> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::vector<std::shared_ptr<Entity2D>> entitiesToRemove;

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
      /// Simple accumulator to count to 1 second
      float oneSecondTimer;
      /// The time at which the previous frame was run
      std::chrono::time_point<std::chrono::steady_clock> lastTime;

      /// Physics simulator
      Physics::Physics2D *physics2D;
      /// Timer used to check if the game is ready for the next physics update
      float physicsAccumulator;

      /// Whether the main thread is finished and ready to sync with the render thread
      bool updateFinished;
      /// Whether the render thread is finished and ready to sync with the main thread
      bool renderFinished;
      /// Mutex to prevent race conditions between the update and render thread
      std::mutex syncMutex;
      /// Variable that allows data to switch threads
      std::condition_variable cv;
      /// The thread responsible for rendering the game
      std::thread renderThread;

      std::shared_ptr<Entity2D> mainCamera;

      /// The update loop called on the main thread
      void updateLoop();

      #if MULTI_THREAD
      /// The render loop called on the render thread
      void renderLoop();
      #endif

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
      void limitFrameRate() const;
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
