//
// Game2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <iostream>
#include <random>
#include <thread>

#include "Engine2D/Game2D.h"
#include "Engine2D/ResourceManager.h"
#include "Engine2D/Physics/Physics2D.h"
#include "Engine2D/Rendering/Shader.h"
#include "Engine2D/Rendering/SpriteRenderer.h"
#include "Common/Macros.h"
#include "Common/Settings.h"
#include "Engine2D/ParticleSystem/ParticleSystem2D.h"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.h"
#include "Input/Gamepad.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace Engine2D {
  Game2D *Game2D::instance = nullptr;
  const float Game2D::screenScaleFactor{0.1f};

  Game2D::Game2D(const int width, const int height, const char *title)
    : aspectRatio(Vector2f::One), title(title), width(width), height(height), window(nullptr),
      deltaTime(0), timeScale(1), targetFrameRate(0), targetRenderRate(0), currentFrameNeedsRendering(false),
      physics2D(nullptr), physicsAccumulator(0) {
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

  Vector2f Game2D::AspectRatio() {
    return instance->aspectRatio;
  }

  float Game2D::DeltaTime() {
    return instance->deltaTime;
  }

  float Game2D::FixedDeltaTime() {
    return Engine::Settings::Physics::GetFixedDeltaTime();
  }

  // Variables for FPS calculation
  auto nextFrameTime = std::chrono::high_resolution_clock::now();
  float oneSecondTimer = 0.0f;
  int frameCounter = 0;

  void Game2D::Run() {
    this->initialize();

    // Set the render variables
    targetFrameRate = Engine::Settings::Graphics::GetTargetFrameRate();
    targetRenderRate = targetFrameRate == 0 ? 0.0f : 1.0f / targetFrameRate;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
      // Calculate the current deltaTime
      auto currentFrameTime = std::chrono::high_resolution_clock::now();
      this->deltaTime = std::chrono::duration<float>(currentFrameTime - lastTime).count() * timeScale;
      lastTime = currentFrameTime;
      glfwPollEvents();

      this->addEntities();
      this->removeEntities();

      this->resetFlags();
      this->processInput();
      this->fixedUpdate();
      this->update();
      this->render();
      this->limitFrameRate();

      // FPS calculation
      oneSecondTimer += deltaTime;
      while (oneSecondTimer >= 1.0f) {
        std::cout << "FPS: " << frameCounter;
        if (Engine::Settings::Graphics::GetFrameSkippingEnabled())
          std::cout << " (Only necessary frames rendered)";
        std::cout << std::endl;
        oneSecondTimer -= 1.0f;
        frameCounter = 0;
        physicsAccumulator = 0.0f;
      }
    }

    this->quit();
  }

  void Game2D::SetGameResourceLoader(ResourceLoader resourceLoader) {
    this->resourceLoader = std::move(resourceLoader);
  }

  void Game2D::Close(const Engine::Input::KeyboardAndMouseContext context) {
    if (context.released)
      glfwSetWindowShouldClose(instance->window, true);
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
    glfwSetWindowRefreshCallback(window, window_refresh_callback);
    glfwSetWindowPosCallback(window, window_pos_callback);

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
    ResourceManager::LoadShader("EngineInclude/Shaders/sprite.vert", "EngineInclude/Shaders/sprite.frag", "", "sprite");
    ResourceManager::GetShader("sprite")->SetInteger("sprite", 0, true);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection);
    Rendering::SpriteRenderer::shader = ResourceManager::GetShader("sprite");
    Rendering::SpriteRenderer::initRenderData();

    // Configure the particle system shader
    ResourceManager::LoadShader(
      "EngineInclude/Shaders/particle.vert", "EngineInclude/Shaders/particle.frag", "", "particle"
    );
    ResourceManager::GetShader("particle")->SetInteger("sprite", 0, true);
    ResourceManager::GetShader("particle")->SetMatrix4("projection", projection);

    ParticleSystemRenderer2D::initialize();

    physics2D = new Physics2D();

    this->OnInitialize();
  }

  void Game2D::resetFlags() {
    // Reset the update flags of the entities
    for (const auto &entity: entities)
      if (entity && entity->IsActive())
        entity->transform.wasUpdated = false;
    currentFrameNeedsRendering = false;
  }

  void Game2D::processInput() {
    if (Engine::Settings::Input::GetAllowMouseInput())
      Engine::Input::Mouse::processInput();
    if (Engine::Settings::Input::GetAllowKeyboardInput())
      Engine::Input::Keyboard::processInput();
    if (Engine::Settings::Input::GetAllowGamepadInput())
      Engine::Input::Gamepad::processInput();
  }

  void Game2D::update() {
    bool foundNull = false;
    for (const auto &entity: entities) {
      if (entity && entity->IsActive()) {
        entity->OnUpdate();
        for (const auto &component: entity->components)
          if (component->IsActive())
            component->OnUpdate();
        currentFrameNeedsRendering = currentFrameNeedsRendering || entity->transform.wasUpdated;
      } else if (!entity)
        foundNull = true;
    }
    if (foundNull)
      for (auto it = entities.begin(); it != entities.end();) {
        if (*it == nullptr)
          it = entities.erase(it);
        else
          ++it;
      }
  }

  void Game2D::fixedUpdate() {
    physicsAccumulator += deltaTime;
    const float fixedDeltaTime = Engine::Settings::Physics::GetFixedDeltaTime();
    while (physicsAccumulator >= fixedDeltaTime) {
      for (const auto &entity: entities)
        if (entity->IsActive()) {
          entity->OnFixedUpdate();
          for (const auto &component: entity->components)
            if (component->IsActive())
              component->OnFixedUpdate();
        }
      physics2D->step();
      physicsAccumulator -= fixedDeltaTime;
    }
  }

  void Game2D::render(const bool override) const {
    // Make sure there is something to render
    if (!entities.empty() && !entitiesToRender.empty()) {
      // Check whether we should skip duplicate frame
      if (override || (!Engine::Settings::Graphics::GetFrameSkippingEnabled() || (
                         Engine::Settings::Graphics::GetFrameSkippingEnabled() && currentFrameNeedsRendering))) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Rendering::SpriteRenderer::render(entitiesToRender);
        ParticleSystemRenderer2D::render();

        // Prepare the next frame
        glfwSwapBuffers(window);
        frameCounter++;
      }
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
      entities.insert(entity);
      if (entity->texture)
        entitiesToRender[entity->texture->id].insert(entity);
      entity->initialize();
    }
    entitiesToAdd.clear();
  }

  void Game2D::removeEntities() {
    for (auto it = instance->entitiesToRemove.begin(); it != instance->entitiesToRemove.end();) {
      auto entity = *it;

      // If the entity has a texture, remove it from the draw list
      if (entity->texture)
        entitiesToRender[entity->texture->id].erase(entity);

      // Remove the entity from the game
      if (entities.erase(entity) > 0)
        entity->destroy();

      // Erase the entity and update the iterator
      it = instance->entitiesToRemove.erase(it);
    }
  }

  void Game2D::removeEntity(const std::shared_ptr<Entity2D> &entity) {
    if (entity && instance && !instance->entities.empty())
      instance->entitiesToRemove.insert(entity);
  }

  void Game2D::framebuffer_size_callback(GLFWwindow *window, const int, const int) {
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    instance->width = framebufferWidth;
    instance->height = framebufferHeight;

    const Vector2<size_t> initialSize = Engine::Settings::Window::GetScreenResolution();

    // Calculate the proper aspect ratio
    const float ratioX = static_cast<float>(framebufferWidth) / static_cast<float>(initialSize.x);
    const float ratioY = static_cast<float>(framebufferHeight) / static_cast<float>(initialSize.y);
    instance->aspectRatio = Vector2{ratioX, ratioY};

    const bool maintainAspectRatio = Engine::Settings::Graphics::GetMaintainAspectRatio();
    const auto viewportRatio = Vector2{
      maintainAspectRatio ? std::min(ratioX, ratioY) : ratioX,
      maintainAspectRatio ? std::min(ratioX, ratioY) : ratioY
    };

    // Calculate the viewport dimensions
    const int viewWidth = static_cast<int>(static_cast<float>(initialSize.x) * viewportRatio.x);
    const int viewHeight = static_cast<int>(static_cast<float>(initialSize.y) * viewportRatio.y);

    // Center the viewport
    glViewport((framebufferWidth - viewWidth) >> 1, (framebufferHeight - viewHeight) >> 1, viewWidth, viewHeight);
  }

  void Game2D::window_refresh_callback(GLFWwindow *) {
    instance->render(true);
  }

  void Game2D::window_pos_callback(GLFWwindow *window, int, int) {
    framebuffer_size_callback(window, instance->width, instance->height);
    instance->render(true);
  }

  void Game2D::scroll_callback(GLFWwindow *, double, const double yOffset) {
    if (Engine::Settings::Input::GetAllowMouseInput())
      Engine::Input::Mouse::processScroll(yOffset);
  }
}
