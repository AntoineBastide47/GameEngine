//
// Game2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <thread>
#include <unordered_set>
#include <cmrc/cmrc.hpp>

#include "Engine/RenderingHeaders.hpp"
#include "Engine/Input/InputContexts.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Game2D.gen.hpp"

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Engine2D::Animation {
  class AnimationSystem;
}

namespace Engine {
  class ResourceManager;
  class Settings;
}

namespace Engine2D {
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D : public Engine::Reflection::Reflectable {
    SERIALIZE_GAME2D
      friend class Entity2D;
      friend class Engine::Settings;
      friend class Engine::ResourceManager;
    public:
      /// @returns True if the Game2D instance has been initialized, False if not
      [[nodiscard]] static bool Initialized();
      /// @returns The width of the window's viewport
      [[nodiscard]] static float ViewportWidth();
      /// @returns The height of the window's viewport
      [[nodiscard]] static float ViewportHeight();
      /// @returns The size of the window's viewport
      [[nodiscard]] static glm::vec2 ViewportSize();
      /// @returns The aspect ratio of the viewport
      [[nodiscard]] static glm::vec2 AspectRatio();
      /// @returns The time span between the current frame and the last frame
      [[nodiscard]] static float DeltaTime();
      /// @returns The fixed time span between the physics updates
      [[nodiscard]] static float FixedDeltaTime();
      /// @returns A reference to the main camera of the game
      [[nodiscard]] static Rendering::Camera2D *MainCamera();

      /**
       * Set's the given resource loader of the game to load embedded resources
       * @param resourceLoader The resource loader that contains the resources to load
       * @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
       */
      void SetGameResourceLoader(ResourceLoader resourceLoader);

      /// Closes/Quits the game
      static void Close(Engine::Input::KeyboardAndMouseContext context);
      /// Creates an entity of with the given name
      static Entity2D *AddEntity(
        const std::string &name = "Entity", bool isStatic = false, glm::vec2 position = glm::vec2(0.0f, 0.0f),
        float rotation = 0.0f, glm::vec2 scale = glm::vec2(1.0f, 1.0f), Entity2D *parent = nullptr
      );
      /// @returns The entity with the given name if it was found, nullptr if not
      static Entity2D *Find(const std::string &name);
      /**
       * Start's and Run's the current game
       * @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
       */
      void Run();
    protected:
      Game2D();

      /**
       * Creates a game
       * @param width The width of the game window
       * @param height The height of the game window
       * @param title The title of the game window
       */
      Game2D(int width, int height, const char *title);

      ~Game2D();

      /// Called during initialization, allowing derived classes to customize behavior.
      virtual void OnInitialize() {}
    private:
      /// Used to resize the coordinate space. If the initial window is 1920x1080 and factor is 0.1f, the coordinate space will
      /// be 192x108.
      static constexpr float screenScaleFactor = 0.1f;
      /// The current aspect ratio of the window
      glm::vec2 aspectRatio;
      /// The inverse of the current window's aspect ratio
      glm::vec2 aspectRatioInv;

      /// The title of the game window
      const char *title;
      /// The width of the game window
      int width;
      /// The height of the game window
      int height;
      /// The pointer to the game window
      GLFWwindow *window;

      /// The current game instance, unique
      inline static Game2D *instance;
      /// The resource loader for embedded resources
      ResourceLoader resourceLoader;
      /// The main camera of the game
      Rendering::Camera2D *cameraComponent;

      /// All the entities currently in the game
      std::vector<std::unique_ptr<Entity2D>> entities;
      /// Entities scheduled to be added to the game
      std::vector<std::unique_ptr<Entity2D>> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::unordered_set<Entity2D *> entitiesToRemove;
      #if MULTI_THREAD
      /// Entities that should be removed from memory
      std::unordered_set<Entity2D *> entitiesToDestroy;
      #endif

      /// The time during two frames
      float deltaTime;
      /// The time scaling factor
      float timeScale;
      /// The frame rate targeted by the engine
      float targetFrameRate;
      /// The rate at which the game should update
      float targetRenderRate;
      /// Counts the number of frames rendered during the current second of elapsed time
      int frameCounter;
      /// Simple accumulator to count to 1 second
      float oneSecondTimer;
      /// The time at which the previous frame was run
      std::chrono::time_point<std::chrono::steady_clock> lastTime;

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

      /// The update loop called on the main thread
      void updateLoop();

      #if MULTI_THREAD
      /// The render loop called on the render thread
      void renderLoop();
      #endif

      /// Initializes the game
      void initialize();
      /// Processes all the inputs to the game
      static void processInput();
      /// Update the game
      void update();
      /// Simulates a step of the physics simulation
      void fixedUpdate();
      /// Animates all entities
      static void animate();
      /// Renders the game
      void render() const;
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
      static void removeEntity(Entity2D *entity);

      // OpenGL callbacks
      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
  };
} // Engine2D

#endif //GAME2D_H
