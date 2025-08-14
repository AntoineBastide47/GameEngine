//
// Game2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <iostream>
#include <random>
#include <ranges>

#include "Engine2D/Game2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Settings.hpp"
#include "Engine/Input/Gamepad.hpp"
#include "Engine/Input/Keyboard.hpp"
#include "Engine/Input/Mouse.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#if ENGINE_PROFILING
#include "Engine/Profiling/Instrumentor.hpp"
#endif
#include "Engine/Macros/Profiling.hpp"
#include "Engine/Reflection/Serializer.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"
#if ENGINE_EDITOR
#include <imgui.h>
#endif

using Engine::ResourceManager;

namespace Engine2D {
  Game2D::Game2D()
    : Game2D(800, 600, "Unnamed Project") {}

  Game2D::Game2D(const int width, const int height, const char *title)
    : aspectRatio(glm::vec2(1)), aspectRatioInv(glm::vec2(1)), title(title), width(width), height(height),
      window(nullptr), deltaTime(0), timeScale(1), targetFrameRate(0), targetRenderRate(0), frameCounter(0),
      oneSecondTimer(0), physicsAccumulator(0), headlessMode(false)
      #if MULTI_THREAD
    , updateFinished(false), renderFinished(true), renderThreadCallback(nullptr), callbackPending(false)
  #endif
  {
    if (instance)
      throw std::runtime_error("ERROR::GAME2D: There can only be one instance of Game2D running.");
    if (width <= 0 || height <= 0)
      throw std::invalid_argument("ERROR::GAME2D: Game window size must be greater than zero");
    instance = this;
  }

  Game2D::~Game2D() {
    quit();
  }

  bool Game2D::Initialized() {
    return instance;
  }

  float Game2D::ViewportWidth() {
    return screenScaleFactor * static_cast<float>(instance->width) * instance->aspectRatioInv.x;
  }

  float Game2D::ViewportHeight() {
    return screenScaleFactor * static_cast<float>(instance->height) * instance->aspectRatioInv.y;
  }

  float Game2D::WindowWidth() {
    return static_cast<float>(instance->width);
  }

  float Game2D::WindowHeight() {
    return static_cast<float>(instance->height);
  }

  glm::vec2 Game2D::ViewportSize() {
    return screenScaleFactor * glm::vec2(instance->width, instance->height) * instance->aspectRatioInv;
  }

  glm::vec2 Game2D::AspectRatio() {
    return instance->aspectRatio;
  }

  float Game2D::DeltaTime() {
    return instance->deltaTime;
  }

  float Game2D::FixedDeltaTime() {
    return Engine::Settings::Physics::GetFixedDeltaTime();
  }

  void Game2D::SetGameResourceLoader(ResourceLoader resourceLoader) {
    this->resourceLoader = std::move(resourceLoader);
  }

  void Game2D::Quit() {
    if (!instance->IsHeadless()) {
      glfwSetWindowShouldClose(instance->window, true);
    }
  }

  void Game2D::Run() {
    #ifdef ENGINE_PROFILING
    Engine::Profiling::Instrumentor::get().beginSession("profiler");
    #endif

    if (!instance) {
      std::cout << "ERROR::GAME2D::Run(): Game window not initialized!" << std::endl;
      exit(EXIT_FAILURE);
    }

    // Set the render variables
    oneSecondTimer = 0.0f;
    lastTime = std::chrono::steady_clock::now();
    targetFrameRate = Engine::Settings::Graphics::GetTargetFrameRate();
    targetRenderRate = targetFrameRate == 0 ? 0.0f : 1.0f / targetFrameRate;
    frameCounter = 0;

    initializeGraphicPipeline();
    initializeGamePipeline(instance->window);

    #if MULTI_THREAD
    // IMPORTANT:
    // GLFW requires event processing (glfwPollEvents) in the main thread.
    // So we run the update loop on the main thread.
    // We then release the GL context so the render thread can make it current.
    glfwMakeContextCurrent(nullptr);
    renderThread = std::thread(&Game2D::renderLoop, this);
    #else
    glfwMakeContextCurrent(window);
    #endif

    updateLoop();

    #if MULTI_THREAD
    renderThread.join();
    #endif

    quit();

    #ifdef ENGINE_PROFILING
    Engine::Profiling::Instrumentor::get().endSession();
    #endif
  }

  void Game2D::initializeGraphicPipeline() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    // Initialize and Configure GLFW
    if (!glfwInit()) {
      std::cerr << "ERROR: GLFW could not initialize" << std::endl;
      quit();
      exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Set window hints
    glfwWindowHint(GLFW_RESIZABLE, Engine::Settings::Window::GetAllowResize());

    // Create the window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
      std::cerr << "ERROR: Failed to create window" << std::endl;
      quit();
      exit(EXIT_FAILURE);
    }
    glViewport(0, 0, width, height);

    // Set window callbacks
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(Engine::Settings::Graphics::GetVsyncEnabled());

    // The official code for "Setting Your Raster Position to a Pixel Location" (i.e., set up a camera for 2D screen)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Make some OpenGL properties better for 2D and enable blending
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(32768.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (const GLenum err = glewInit(); err != GLEW_OK) {
      std::cerr << "ERROR: Failed to initialize GLEW: " << err << std::endl;
      quit();
      exit(EXIT_FAILURE);
    }
  }

  void Game2D::initializeGamePipeline(GLFWwindow *window) {
    Engine::Input::Mouse::initialize(window);
    Engine::Input::Keyboard::initialize(window);

    // Create the default scene
    SceneManager::CreateScene("default");
    SceneManager::SetActiveScene("default");

    // Set up the shader preprocessor and load the engine shaders
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Rendering::Renderer2D::MAX_TEXTURES);
    ResourceManager::LoadShader("sprite", "Engine/Shaders/sprite.glsl");

    // Initialize timing
    lastTime = std::chrono::steady_clock::now();
    deltaTime = 0.016f;

    OnInitialize();
  }

  void Game2D::updateLoop() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerThread);

    while (!glfwWindowShouldClose(window)) {
      // Calculate the current deltaTime
      const auto currentFrameTime = std::chrono::high_resolution_clock::now();
      deltaTime = std::chrono::duration<float>(currentFrameTime - lastTime).count() * timeScale;
      lastTime = currentFrameTime;
      glfwPollEvents();

      processInput();
      if (const auto scene = SceneManager::ActiveScene(); scene) {
        scene->syncEntities();
        scene->update();
        scene->fixedUpdate();
        scene->animate();
        #if !MULTI_THREAD
        scene->render();
        #endif
      }

      #if MULTI_THREAD
      // Handoff engine data to the render thread
      {
        std::unique_lock lock(syncMutex);
        cv.wait(
          lock, [this] {
            return renderFinished;
          }
        );
        updateFinished = true;
        renderFinished = false;
      }
      cv.notify_one();
      #endif

      // FPS calculation
      frameCounter++;
      limitFrameRate();
      oneSecondTimer += deltaTime;
      while (oneSecondTimer >= 1.0f) {
        const std::string fpsString = std::to_string(frameCounter);
        std::cout.write("FPS: ", 5);
        std::cout.write(fpsString.c_str(), fpsString.length());
        std::cout.put('\n');
        oneSecondTimer -= 1.0f;
        frameCounter = 0;
        physicsAccumulator = 0.0f;
      }
    }

    #if MULTI_THREAD
    // In case the window is closing, ensure the render thread isn’t left waiting.
    {
      std::unique_lock lock(syncMutex);
      updateFinished = true;
    }
    cv.notify_one();
    #endif
  }

  #if MULTI_THREAD
  void Game2D::renderLoop() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerThread);

    if (!instance) {
      cv.notify_one();
      updateFinished = false;
      renderFinished = true;
      return;
    }

    // Make the GL context current in this thread.
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
      std::unique_lock lock(syncMutex);
      cv.wait(
        lock, [this] {
          return updateFinished || callbackPending;
        }
      );

      // Run render thread callback
      if (callbackPending && renderThreadCallback) {
        renderThreadCallback();
        renderThreadCallback = nullptr;
        callbackPending = false;
        controlCV.notify_one();
        continue;
      }

      // Now the engine data is exclusively ours; render the frame.
      if (SceneManager::ActiveScene())
        SceneManager::ActiveScene()->render();

      updateFinished = false;
      renderFinished = true;
      cv.notify_one();
    }
  }
  #endif

  void Game2D::processInput() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    if (Engine::Settings::Input::GetAllowMouseInput())
      Engine::Input::Mouse::processInput();
    if (Engine::Settings::Input::GetAllowKeyboardInput())
      Engine::Input::Keyboard::processInput();
    if (Engine::Settings::Input::GetAllowGamepadInput())
      Engine::Input::Gamepad::processInput();
  }

  void Game2D::limitFrameRate() const {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    using namespace std::chrono;

    if (Engine::Settings::Graphics::GetVsyncEnabled())
      return;

    static auto nextFrameTime = steady_clock::now();
    nextFrameTime += duration_cast<steady_clock::duration>(duration<float>(targetRenderRate));

    if (const auto now = steady_clock::now(); nextFrameTime > now) {
      // sleep coarse then spin-wait
      if (const auto sleepTime = nextFrameTime - now - microseconds(200); sleepTime > microseconds(0))
        std::this_thread::sleep_for(sleepTime);
      while (steady_clock::now() < nextFrameTime);
    } else
      nextFrameTime = now;
  }

  void Game2D::quit() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    // Deallocate all the game resources
    SceneManager::DestroyAllScenes();
    ResourceManager::Clear();
    Engine::Reflection::ReflectionFactory::cleanup();

    // Destroy the window and terminate all OpenGL processes
    if (window) {
      glfwDestroyWindow(window);
      window = nullptr;
      glfwTerminate();
    }

    instance = nullptr;
  }

  cmrc::file Game2D::loadResource(const std::string &path) const {
    if (resourceLoader)
      return resourceLoader(path);
    throw std::runtime_error("ERROR::GAME2D: Resource loader not set.");
  }

  void Game2D::framebuffer_size_callback(GLFWwindow *, const int width, const int height) {
    if (!instance)
      return;

    instance->width = width;
    instance->height = height;

    const glm::vec<2, size_t> initialSize = Engine::Settings::Window::GetScreenResolution();

    // Calculate the proper aspect ratio
    const float ratioX = static_cast<float>(width) / static_cast<float>(initialSize.x);
    const float ratioY = static_cast<float>(height) / static_cast<float>(initialSize.y);
    instance->aspectRatio = glm::vec2(ratioX, ratioY);
    instance->aspectRatioInv = 1.0f / instance->aspectRatio;

    // Center the viewport
    glViewport(0, 0, width, height);

    if (Engine::Settings::Graphics::GetMaintainAspectRatio())
      if (auto *camera = SceneManager::ActiveScene()->MainCamera())
        camera->Resize(width, height);
  }

  void Game2D::scroll_callback(GLFWwindow *, double, const double yOffset) {
    if (Engine::Settings::Input::GetAllowMouseInput())
      Engine::Input::Mouse::processScroll(yOffset);
  }

  void Game2D::window_close_callback(GLFWwindow *) {
    Quit();
  }

  void Game2D::updateGame() {
    if (!headlessMode)
      return;

    // Calculate delta time
    const auto currentFrameTime = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<float>(currentFrameTime - lastTime).count() * timeScale;
    lastTime = currentFrameTime;

    // Update the active scene
    if (const auto scene = SceneManager::ActiveScene(); scene) {
      scene->syncEntities();
      scene->update();
      scene->fixedUpdate();
      scene->animate();
    }
  }

  void Game2D::renderFrame() const {
    if (!headlessMode)
      return;

    // Render the active scene to the current framebuffer
    if (const auto scene = SceneManager::ActiveScene(); scene) {
      scene->renderHeadless();
    }
  }

  void Game2D::setAsHeadless() {
    headlessMode = true;
  }
}
