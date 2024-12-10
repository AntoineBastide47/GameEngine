//
// Game2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <iostream>
#include <random>
#include <thread>

#include "2D/Game2D.h"
#include "2D/ResourceManager.h"
#include "2D/Physics/Rigidbody2D.h"
#include "2D/Rendering/ShapeRenderer.h"
#include "2D/Rendering/SpriteRenderer.h"
#include "Common/Macros.h"
#include "Common/RenderingHeaders.h"
#include "Input/Gamepad.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace Engine2D {
  Game2D *Game2D::instance = nullptr;
  const float Game2D::screenScaleFactor{0.1f};

  Game2D::Game2D(const int width, const int height, std::string title)
    : width(width), height(height), initialWidth(width), initialHeight(height), aspectRatio(Vector2::One),
      title(std::move(title)), window(nullptr), deltaTime(0), root(new Entity2D("Root")), physics2D(nullptr),
      frameRate(0), renderRate(0), physicsAccumulator(0), physicsStepCount(0) {
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

  Vector2 Game2D::AspectRatio() {
    return instance->aspectRatio;
  }

  float Game2D::DeltaTime() {
    return instance->deltaTime;
  }

  float Game2D::FixedDeltaTime() {
    return instance->physicsTimeStep;
  }

  Game2D *Game2D::Instance() {
    return instance;
  }

  void Game2D::Run() {
    this->initialize();

    // Variables for FPS calculation
    auto nextFrameTime = std::chrono::high_resolution_clock::now();
    renderRate = this->frameRate == 0.0f ? 0.0f : 1.0f / this->frameRate;
    float oneSecondTimer = 0.0f;
    float frameCounter = 0;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
      auto currentFrameTime = std::chrono::high_resolution_clock::now();
      this->deltaTime = std::chrono::duration<float>(currentFrameTime - lastTime).count();
      lastTime = currentFrameTime;
      glfwPollEvents();

      // Process the inputs to the game
      Engine::Input::Keyboard::processInput();
      Engine::Input::Mouse::processInput();
      Engine::Input::Gamepad::processInput();

      // Update, Simulate and Render the game
      this->AddEntities();
      this->update();
      this->fixedUpdate();
      this->render();
      this->RemoveEntities();

      // FPS calculation
      oneSecondTimer += deltaTime;
      frameCounter++;
      if (oneSecondTimer >= 1.0f) {
        std::cout << "FPS: " << frameCounter << std::endl;
        oneSecondTimer -= 1;
        frameCounter = 0;
        physicsStepCount = 0;
      }

      nextFrameTime += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<float>(renderRate)
      );
      std::this_thread::sleep_until(nextFrameTime);
    }

    this->quit();
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
    constexpr bool allowWindowResize = true; // TODO: make this an option in settings
    glfwWindowHint(GLFW_RESIZABLE, allowWindowResize);

    // Create the window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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

    constexpr bool vsyncEnabled = false; // TODO: make this an option in settings
    glfwSwapInterval(vsyncEnabled);

    // The official code for "Setting Your Raster Position to a Pixel Location" (i.e. set up a camera for 2D screen)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Make some OpenGL properties better for 2D and enable alpha channel.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

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

    // Create and configure the sprite renderer
    ResourceManager::LoadShader("EngineFiles/Shaders/sprite.vs", "EngineFiles/Shaders/sprite.fs", "", "sprite");
    const glm::mat4 projection = glm::ortho(
      -static_cast<float>(width) * screenScaleFactor * 0.5f, static_cast<float>(width) * screenScaleFactor * 0.5f,
      -static_cast<float>(height) * screenScaleFactor * 0.5f, static_cast<float>(height) * screenScaleFactor * 0.5f,
      -1.0f, 1.0f
    );
    ResourceManager::GetShader("sprite")->SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection, true);
    Rendering::SpriteRenderer::shader = ResourceManager::GetShader("sprite");
    Rendering::SpriteRenderer::initRenderData();

    // Create and configure the shape renderer
    ResourceManager::LoadShader("EngineFiles/Shaders/shape.vs", "EngineFiles/Shaders/shape.fs", "", "shape");
    ResourceManager::GetShader("shape")->SetMatrix4("projection", projection, true);
    Rendering::ShapeRenderer::shader = ResourceManager::GetShader("shape");
    Rendering::ShapeRenderer::initRenderData();

    physics2D = new Physics::Physics2D(physicsTimeStep);

    this->Initialize();
  }

  void Game2D::update() {
    this->Update();
    for (const auto &entity: entities)
      if (entity->active)
        entity->update();
  }

  void Game2D::fixedUpdate() {
    physicsAccumulator += deltaTime;
    while (maxPhysicsStepsPerSecond > physicsStepCount && physicsAccumulator > physicsTimeStep) {
      for (const auto &entity: entities)
        if (entity->active)
          entity->FixedUpdate();
      physics2D->step();
      physicsAccumulator -= physicsTimeStep;
      physicsStepCount++;
    }
  }

  void Game2D::render() const {
    // Clear previous frame
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw all the entities
    Rendering::SpriteRenderer::shader->Use();
    for (const auto &entity: entities)
      if (entity && entity->active && entity->texture)
        Rendering::SpriteRenderer::DrawSprite(entity);
    // Prepare the next frame
    glfwSwapBuffers(window);
    glFinish();
  }

  void Game2D::quit() {
    this->Quit();

    // Destroy the window and terminate all OpenGL processes
    if (window) {
      glfwDestroyWindow(window);
      window = nullptr;
    }
    glfwTerminate();

    // Remove all the entities from the game
    for (const auto &entity: entities)
      RemoveEntity(entity);
    entities.clear();

    // Deallocate all the game resources
    ResourceManager::Clear();
    SAFE_DELETE(physics2D);
    SAFE_DELETE(root);
    instance = nullptr;
  }

  void Game2D::SetGameResourceLoader(const ResourceLoader &resourceLoader) {
    this->resourceLoader = resourceLoader;
  }

  void Game2D::Close(const Engine::Input::KeyboardAndMouseContext context) {
    if (context.released)
      glfwSetWindowShouldClose(instance->window, true);
  }

  cmrc::file Game2D::loadResource(const std::string &path) const {
    if (resourceLoader)
      return resourceLoader(path);
    throw std::runtime_error("ERROR::GAME2D: Resource loader not set.");
  }

  void Game2D::AddEntities() {
    for (auto &entity: entitiesToAdd)
      entities.push_back(entity);
    entitiesToAdd.clear();
  }

  void Game2D::AddEntity(Entity2D *entity) {
    if (!entity || !instance || !instance->root || entity == instance->root)
      return;

    // Check if the entity is already in the list
    for (const auto &e: instance->entities)
      if (*e == *entity)
        return;

    instance->entitiesToAdd.push_back(entity);
  }

  void Game2D::RemoveEntities() {
    for (auto &entity: entitiesToRemove) {
      const auto it = std::ranges::find_if(
        entities,
        [&entity](const Entity2D *ptr) {
          return ptr == entity;
        }
      );
      if (it != entities.end()) {
        entities.erase(it);
        entity->Quit();
      }
    }
    instance->entitiesToRemove.clear();
  }

  void Game2D::RemoveEntity(Entity2D *entity) {
    if (entity && instance && entity != instance->root && !instance->entities.empty())
      instance->entitiesToRemove.push_back(entity);
  }

  void Game2D::framebuffer_size_callback(GLFWwindow *window, const int, const int) {
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    instance->width = framebufferWidth;
    instance->height = framebufferHeight;

    // Calculate the proper aspect ratio
    const float ratioX = static_cast<float>(framebufferWidth) / static_cast<float>(instance->initialWidth);
    const float ratioY = static_cast<float>(framebufferHeight) / static_cast<float>(instance->initialHeight);
    instance->aspectRatio = Vector2{ratioX, ratioY};

    constexpr bool maintainAspectRatio = true; // TODO: make this an option in settings
    const auto viewportRatio = Vector2{
      maintainAspectRatio ? std::min(ratioX, ratioY) : ratioX,
      maintainAspectRatio ? std::min(ratioX, ratioY) : ratioY
    };

    // Calculate the viewport dimensions
    const int viewWidth = static_cast<int>(static_cast<float>(instance->initialWidth) * viewportRatio.x);
    const int viewHeight = static_cast<int>(static_cast<float>(instance->initialHeight) * viewportRatio.y);

    // Center the viewport
    glViewport((framebufferWidth - viewWidth) >> 1, (framebufferHeight - viewHeight) >> 1, viewWidth, viewHeight);
  }

  void Game2D::window_refresh_callback(GLFWwindow *) {
    instance->render();
  }

  void Game2D::window_pos_callback(GLFWwindow *window, int, int) {
    framebuffer_size_callback(window, instance->width, instance->height);
    instance->render();
  }

  void Game2D::scroll_callback(GLFWwindow *, double, const double yOffset) {
    Engine::Input::Mouse::processScroll(yOffset);
  }
}
