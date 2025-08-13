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

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Editor {
  class Window;
  class LevelEditor;
}

namespace Engine {
  class Settings;
  class ResourceManager;
}

namespace Engine2D {
  namespace Animation {
    class AnimationSystem;
  }

  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Scene;
    friend class Entity2D;
    friend class SceneManager;
    friend class Editor::Window;
    friend class Engine::Settings;
    friend class Editor::LevelEditor;
    friend class Engine::ResourceManager;
    public:
      /// @returns True if the Game2D instance has been initialized, False if not
      [[nodiscard]] static bool Initialized();
      /// @returns The width of the window's viewport
      [[nodiscard]] static float ViewportWidth();
      /// @returns The height of the window's viewport
      [[nodiscard]] static float ViewportHeight();
      /// @returns The width of the window's
      [[nodiscard]] static float WindowWidth();
      /// @returns The height of the window's
      [[nodiscard]] static float WindowHeight();
      /// @returns The size of the window's viewport
      [[nodiscard]] static glm::vec2 ViewportSize();
      /// @returns The aspect ratio of the viewport
      [[nodiscard]] static glm::vec2 AspectRatio();
      /// @returns The time span between the current frame and the last frame
      [[nodiscard]] static float DeltaTime();
      /// @returns The fixed time span between the physics updates
      [[nodiscard]] static float FixedDeltaTime();

      /// Set's the given resource loader of the game to load embedded resources
      /// @param resourceLoader The resource loader that contains the resources to load
      /// @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
      void SetGameResourceLoader(ResourceLoader resourceLoader);

      /// Quits the game
      static void Quit();

      /// Start's and Run's the current game
      /// @note Do not call this function yourself in your code, it will be called in the main.cpp of your game
      void Run();

      virtual ~Game2D();
    protected:
      Game2D();

      /// Creates a game
      /// @param width The width of the game window
      /// @param height The height of the game window
      /// @param title The title of the game window
      Game2D(int width, int height, const char *title);

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

      /// Whether the game is running in headless mode (no window)
      bool headlessMode;

      #if MULTI_THREAD
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

      std::mutex controlMutex;
      std::condition_variable controlCV;
      std::function<void()> renderThreadCallback;
      bool callbackPending;
      #endif

      /// The update loop called on the main thread
      void updateLoop();

      #if MULTI_THREAD
      /// The render loop called on the render thread
      void renderLoop();
      #endif

      /// Initialize's the graphics code
      void initializeGraphicPipeline();
      /// Initialize's the game code
      void initializeGamePipeline(GLFWwindow *window);
      /// Processes all the inputs to the game
      static void processInput();
      /// Limits the frame rate of the game if needed
      void limitFrameRate() const;
      /// Quits the game
      void quit();

      /// Loads the given resource from the embedded resources
      [[nodiscard]] cmrc::file loadResource(const std::string &path) const;

      /// Update game logic without rendering (for editor use)
      void updateGame();
      /// Render a single frame to the current framebuffer (for editor use)
      void renderFrame() const;
      /// Makes this game instance a headless instance of the game
      void setAsHeadless();
      /// Check if running in headless mode
      [[nodiscard]] bool IsHeadless() const {
        return headlessMode;
      }

      // OpenGL callbacks
      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
      static void window_close_callback(GLFWwindow *window);
  };
} // Engine2D

#endif //GAME2D_H
