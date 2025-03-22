//
// Game2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <iostream>
#include <random>

#include "Engine2D/Game2D.hpp"
#include "Engine2D/ResourceManager.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Rendering/Shader.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine/Macros/Utils.hpp"
#include "Engine/Settings.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.hpp"
#include "Engine2D/Rendering/ShapeRenderer.hpp"
#include "Engine/Input/Gamepad.hpp"
#include "Engine/Input/Keyboard.hpp"
#include "Engine/Input/Mouse.hpp"
#include "Engine2D/Behaviour.hpp"

namespace Engine2D {
  Game2D *Game2D::instance = nullptr;
  const float Game2D::screenScaleFactor{0.1f};

  Game2D::Game2D(const int width, const int height, const char *title)
    : aspectRatio(glm::vec2(1)), title(title), width(width), height(height), window(nullptr),
      deltaTime(0), timeScale(1), targetFrameRate(0), targetRenderRate(0), frameCounter(0), physics2D(nullptr),
      physicsAccumulator(0), updateFinished(false), renderFinished(true) {
    if (instance)
      throw std::runtime_error("ERROR::GAME2D: There can only be one instance of Game2D running.");
    if (width <= 0 || height <= 0)
      throw std::invalid_argument("ERROR::GAME2D: Game window size must be greater than zero");
    instance = this;
  }

  float Game2D::ViewportWidth() {
    return screenScaleFactor * static_cast<float>(instance->width) / instance->aspectRatio.x;
  }

  float Game2D::ViewportHeight() {
    return screenScaleFactor * static_cast<float>(instance->height) / instance->aspectRatio.y;
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

  float oneSecondTimer = 0.0f;
  auto lastTime = std::chrono::high_resolution_clock::now();

  void Game2D::Run() {
    this->initialize();

    // Set the render variables
    targetFrameRate = Engine::Settings::Graphics::GetTargetFrameRate();
    targetRenderRate = targetFrameRate == 0 ? 0.0f : 1.0f / targetFrameRate;
    nextFrameTime = std::chrono::high_resolution_clock::now();
    frameCounter = 0;

    // IMPORTANT:
    // GLFW requires event processing (glfwPollEvents) in the main thread.
    // So we run the update loop on the main thread.
    // We then release the GL context so the render thread can make it current.
    glfwMakeContextCurrent(nullptr);

    renderThread = std::thread(&Game2D::renderLoop, this);
    updateLoop();
    renderThread.join();

    this->quit();
  }

  void Game2D::updateLoop() {
    while (!glfwWindowShouldClose(window)) {
      // Calculate the current deltaTime
      const auto currentFrameTime = std::chrono::high_resolution_clock::now();
      this->deltaTime = std::chrono::duration<float>(currentFrameTime - lastTime).count() * timeScale;
      lastTime = currentFrameTime;

      glfwPollEvents();

      this->addEntities();
      this->removeEntities();

      this->processInput();
      this->fixedUpdate();
      this->update();

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

      this->limitFrameRate();

      // FPS calculation
      oneSecondTimer += deltaTime;
      while (oneSecondTimer >= 1.0f) {
        std::cout << "FPS: " << frameCounter << std::endl;
        oneSecondTimer -= 1.0f;
        frameCounter = 0;
        physicsAccumulator = 0.0f;
      }
    }
    // In case the window is closing, ensure the render thread isn’t left waiting.
    {
      std::unique_lock lock(syncMutex);
      updateFinished = true;
    }
    cv.notify_one();
  }

  void Game2D::renderLoop() {
    // Make the GL context current in this thread.
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
      {
        std::unique_lock lock(syncMutex);
        cv.wait(
          lock, [this] {
            return updateFinished;
          }
        );
        // Now the engine data is exclusively ours; render the frame.
        this->render();
        updateFinished = false;
        renderFinished = true;
      }
      cv.notify_one();
    }
  }

  void Game2D::SetGameResourceLoader(ResourceLoader resourceLoader) {
    this->resourceLoader = std::move(resourceLoader);
  }

  void Game2D::Close(const Engine::Input::KeyboardAndMouseContext context) {
    if (context.released)
      glfwSetWindowShouldClose(instance->window, true);
  }

  std::shared_ptr<Entity2D> Game2D::AddEntity(std::string name) {
    if (instance) {
      auto entity = std::make_shared<Entity2D>(name);
      instance->entitiesToAdd.push_back(entity);
      entity->initialize();
      return entity;
    }
    return nullptr;
  }

  std::shared_ptr<Entity2D> Game2D::Find(const std::string &name) {
    for (const auto entity: instance->entitiesToAdd)
      if (entity->name == name)
        return entity;
    for (const auto entity: instance->entities)
      if (entity->name == name)
        return entity;
    return nullptr;
  }

  void Game2D::initialize() {
    // Initialize and Configure GLFW
    if (!glfwInit()) {
      std::cerr << "ERROR: GLFW could not initialize" << std::endl;
      this->quit();
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
      this->quit();
      exit(EXIT_FAILURE);
    }
    glViewport(0, 0, width, height);

    // Set window callbacks
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(Engine::Settings::Graphics::GetVsyncEnabled());

    // The official code for "Setting Your Raster Position to a Pixel Location" (i.e. set up a camera for 2D screen)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Make some OpenGL properties better for 2D and enable alpha channel.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (const GLenum err = glewInit(); err != GLEW_OK) {
      std::cerr << "ERROR: Failed to initialize GLEW: " << err << std::endl;
      this->quit();
      exit(EXIT_FAILURE);
    }

    // Initialize input system
    Engine::Input::Keyboard::initialize(this->window);
    Engine::Input::Mouse::initialize(this->window);

    // Prepare the shader projection
    const glm::mat4 projection = glm::ortho(
      -static_cast<float>(width) * screenScaleFactor * 0.5f, static_cast<float>(width) * screenScaleFactor * 0.5f,
      -static_cast<float>(height) * screenScaleFactor * 0.5f, static_cast<float>(height) * screenScaleFactor * 0.5f,
      -1.0f, 1.0f
    );

    // Create and configure the sprite renderer
    ResourceManager::LoadShader("Engine/Shaders/sprite.vert", "Engine/Shaders/sprite.frag", "", "sprite");
    ResourceManager::GetShader("sprite")->SetInteger("sprite", 0, true);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection);
    Rendering::SpriteRenderer::shader = ResourceManager::GetShader("sprite");

    // Configure the particle system shader
    ResourceManager::LoadShader(
      "Engine/Shaders/particle.vert", "Engine/Shaders/particle.frag", "", "particle"
    );
    ResourceManager::GetShader("particle")->SetInteger("sprite", 0, true);
    ResourceManager::GetShader("particle")->SetMatrix4("projection", projection);

    ResourceManager::LoadShader("Engine/Shaders/shape.vert", "Engine/Shaders/shape.frag", "", "shape");
    ResourceManager::GetShader("shape")->SetMatrix4("projection", projection, true);
    Rendering::ShapeRenderer::shader = ResourceManager::GetShader("shape");

    physics2D = new Physics2D();

    this->OnInitialize();
  }

  // ReSharper disable once CppMemberFunctionMayBeStatic
  void Game2D::processInput() {
    if (Engine::Settings::Input::GetAllowMouseInput())
      Engine::Input::Mouse::processInput();
    if (Engine::Settings::Input::GetAllowKeyboardInput())
      Engine::Input::Keyboard::processInput();
    if (Engine::Settings::Input::GetAllowGamepadInput())
      Engine::Input::Gamepad::processInput();
  }

  void Game2D::fixedUpdate() {
    physicsAccumulator += deltaTime;
    const float fixedDeltaTime = Engine::Settings::Physics::GetFixedDeltaTime();
    while (physicsAccumulator >= fixedDeltaTime) {
      for (const auto &entity: entities)
        if (entity->IsActive())
          for (const auto &behaviour: entity->behaviours)
            behaviour->OnFixedUpdate();
      physics2D->step();
      physicsAccumulator -= fixedDeltaTime;
    }
  }

  void Game2D::update() {
    bool foundNull = false;
    for (const auto &entity: entities) {
      if (entity && entity->IsActive())
        for (const auto &behaviour: entity->behaviours)
          behaviour->OnUpdate();
      else if (!entity)
        foundNull = true;
    }
    if (foundNull)
      for (auto it = entities.begin(); it != entities.end();) {
        if (*it == nullptr)
          it = entities.erase(it);
        else
          ++it;
      }
    ParticleSystemRenderer2D::update();
  }

  void Game2D::render() {
    // Make sure there is something to render
    if (!entities.empty() && !entitiesToRender.empty()) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Rendering::SpriteRenderer::render(entitiesToRender);
      ParticleSystemRenderer2D::render();

      // Prepare the next frame
      glfwSwapBuffers(window);
      frameCounter++;
    }
  }

  void Game2D::limitFrameRate() {
    // Check if vsync is enabled to limit the frame rate
    if (Engine::Settings::Graphics::GetVsyncEnabled()) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      targetRenderRate = 1.0f / mode->refreshRate;
    }

    // Limit the frame rate if needed
    if (targetFrameRate > 0 || Engine::Settings::Graphics::GetVsyncEnabled()) {
      nextFrameTime += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<float>(targetRenderRate)
      );
      if (const auto currentTime = std::chrono::high_resolution_clock::now(); nextFrameTime > currentTime) {
        std::this_thread::sleep_for(nextFrameTime - currentTime - std::chrono::milliseconds(1));
        while (std::chrono::high_resolution_clock::now() < nextFrameTime);
      }
    }
  }

  void Game2D::quit() {
    // Remove all the entities from the game
    entitiesToAdd.clear();
    entitiesToRemove.clear();
    entitiesToRemove.swap(entities);
    removeEntities();
    entities.clear();
    entitiesToRender.clear();

    // Deallocate all the game resources
    ResourceManager::Clear();
    SAFE_DELETE(physics2D);

    // Destroy the window and terminate all OpenGL processes
    if (window) {
      glfwDestroyWindow(window);
      window = nullptr;
    }
    glfwTerminate();

    instance = nullptr;
  }

  cmrc::file Game2D::loadResource(const std::string &path) const {
    if (resourceLoader)
      return resourceLoader(path);
    throw std::runtime_error("ERROR::GAME2D: Resource loader not set.");
  }

  void Game2D::addEntities() {
    for (auto entity: entitiesToAdd) {
      entities.emplace_back(entity);
      if (entity->texture)
        entitiesToRender[entity->texture->id].emplace_back(entity);
      entity->initialize();
    }
    entitiesToAdd.clear();
  }

  void Game2D::removeEntities() {
    for (auto it = instance->entitiesToRemove.begin(); it != instance->entitiesToRemove.end();) {
      const auto entity = *it;

      // If the entity has a texture, remove it from the draw list
      if (entity->texture)
        std::erase(entitiesToRender[entity->texture->id], entity);

      // Remove the entity from the game
      std::erase(entities, entity);
      entity->destroy();

      // Erase the entity and update the iterator
      it = instance->entitiesToRemove.erase(it);
    }
  }

  void Game2D::removeEntity(const std::shared_ptr<Entity2D> &entity) {
    if (entity && instance && !instance->entities.empty())
      instance->entitiesToRemove.emplace_back(entity);
  }

  void Game2D::framebuffer_size_callback(GLFWwindow *window, const int, const int) {
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    instance->width = framebufferWidth;
    instance->height = framebufferHeight;

    const glm::vec<2, size_t> initialSize = Engine::Settings::Window::GetScreenResolution();

    // Calculate the proper aspect ratio
    const float ratioX = static_cast<float>(framebufferWidth) / static_cast<float>(initialSize.x);
    const float ratioY = static_cast<float>(framebufferHeight) / static_cast<float>(initialSize.y);
    instance->aspectRatio = glm::vec2(ratioX, ratioY);
    const bool maintainAspectRatio = Engine::Settings::Graphics::GetMaintainAspectRatio();

    // Calculate the viewport dimensions
    const int viewWidth = static_cast<int>(
      static_cast<float>(initialSize.x) * (maintainAspectRatio ? std::min(ratioX, ratioY) : ratioX)
    );
    const int viewHeight = static_cast<int>(
      static_cast<float>(initialSize.y) * (maintainAspectRatio ? std::min(ratioX, ratioY) : ratioY)
    );

    // Center the viewport
    glViewport((framebufferWidth - viewWidth) / 2, (framebufferHeight - viewHeight) / 2, viewWidth, viewHeight);
  }

  void Game2D::scroll_callback(GLFWwindow *, double, const double yOffset) {
    if (Engine::Settings::Input::GetAllowMouseInput())
      Engine::Input::Mouse::processScroll(yOffset);
  }
}
