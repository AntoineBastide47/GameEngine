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
#include "Common/Settings.h"
#include "Input/Gamepad.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace Engine2D {
  Game2D *Game2D::instance = nullptr;
  const float Game2D::screenScaleFactor{0.1f};

  Game2D::Game2D(const int width, const int height, std::string title)
    : aspectRatio(Vector2f::One), title(std::move(title)), width(width), height(height), window(nullptr),
      root(new Entity2D("Root")), deltaTime(0), timeScale(1), targetRenderRate(0), physics2D(nullptr),
      physicsAccumulator(0) {
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
    return Engine::Settings::Physics.GetFixedDeltaTime();
  }

  void Game2D::Run() {
    this->initialize();

    // Variables for FPS calculation
    targetFrameRate = Engine::Settings::Graphics.GetTargetFrameRate();
    targetRenderRate = targetFrameRate == 0 ? 0.0f : 1.0f / targetFrameRate;
    auto nextFrameTime = std::chrono::high_resolution_clock::now();
    float oneSecondTimer = 0.0f;
    float frameCounter = 0;
    auto acc = 0.0f;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
      // Calculate the current deltaTime
      auto currentFrameTime = std::chrono::high_resolution_clock::now();
      this->deltaTime = std::chrono::duration<float>(currentFrameTime - lastTime).count() * timeScale;
      lastTime = currentFrameTime;
      glfwPollEvents();

      this->addEntities();
      this->processInput();
      this->update();
      auto start = std::chrono::high_resolution_clock::now();
      this->fixedUpdate();
      acc += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).
        count();
      this->removeEntities();
      this->render();

      // FPS calculation
      oneSecondTimer += deltaTime;
      frameCounter++;
      while (oneSecondTimer >= 1.0f) {
        std::cout << "FPS: " << frameCounter << " Total Physics in (μs): " << acc << std::endl;
        oneSecondTimer -= 1.0f;
        frameCounter = 0;
        physicsAccumulator = 0.0f;
        acc = 0.0f;
      }

      // Limit the frame rate if needed
      if (targetFrameRate > 0) {
        nextFrameTime += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
          std::chrono::duration<float>(targetRenderRate)
        );
        if (auto currentTime = std::chrono::high_resolution_clock::now(); nextFrameTime > currentTime) {
          std::this_thread::sleep_for(nextFrameTime - currentTime - std::chrono::milliseconds(1));
          while (std::chrono::high_resolution_clock::now() < nextFrameTime);
        }
      }
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
    glfwWindowHint(GLFW_RESIZABLE, Engine::Settings::Window.GetAllowResize());

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

    glfwSwapInterval(Engine::Settings::Graphics.GetVsyncEnabled());

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

    // Prepare the shader projection
    const glm::mat4 projection = glm::ortho(
      -static_cast<float>(width) * screenScaleFactor * 0.5f, static_cast<float>(width) * screenScaleFactor * 0.5f,
      -static_cast<float>(height) * screenScaleFactor * 0.5f, static_cast<float>(height) * screenScaleFactor * 0.5f,
      -1.0f, 1.0f
    );

    // Create and configure the sprite renderer
    ResourceManager::LoadShader("EngineFiles/Shaders/sprite.vs", "EngineFiles/Shaders/sprite.fs", "", "sprite");
    ResourceManager::GetShader("sprite")->SetInteger("sprite", 0, true);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection);
    Rendering::SpriteRenderer::shader = ResourceManager::GetShader("sprite");
    Rendering::SpriteRenderer::initRenderData();

    // Create and configure the shape renderer
    ResourceManager::LoadShader("EngineFiles/Shaders/shape.vs", "EngineFiles/Shaders/shape.fs", "", "shape");
    ResourceManager::GetShader("shape")->SetMatrix4("projection", projection, true);
    Rendering::ShapeRenderer::shader = ResourceManager::GetShader("shape");
    Rendering::ShapeRenderer::initRenderData();

    physics2D = new Physics::Physics2D();

    this->Initialize();
  }

  void Game2D::processInput() {
    Engine::Input::Keyboard::processInput();
    Engine::Input::Mouse::processInput();
    Engine::Input::Gamepad::processInput();
  }

  void Game2D::update() const {
    for (const auto &entity: entities)
      if (entity->active)
        entity->update();
  }

  void Game2D::fixedUpdate() {
    physicsAccumulator += deltaTime;
    while (physicsAccumulator >= Engine::Settings::Physics.GetFixedDeltaTime()) {
      for (const auto entity: entities)
        if (entity->active)
          entity->FixedUpdate();
      physics2D->step();
      physicsAccumulator -= Engine::Settings::Physics.GetFixedDeltaTime();
    }
  }

  void Game2D::render() const {
    // Clear previous frame
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Rendering::SpriteRenderer::drawSprites(entitiesById);

    /*for (const auto rigidbody: physics2D->rigidbodies) {
      if (!rigidbody->Transform()->IsInScreenBounds())
        continue;
      if (rigidbody->type == Physics::Rigidbody2D::Circle)
        Rendering::ShapeRenderer::DrawCircleWireframe(
          rigidbody->Transform()->WorldPosition(), rigidbody->Transform()->WorldHalfScale().x, glm::vec3(1, 0, 0)
        );
      else {
        Rendering::ShapeRenderer::DrawPolygonWireframe(rigidbody->transformedVertices, glm::vec3(1, 0, 0));
        Rendering::ShapeRenderer::DrawPolygonWireframe(
          {rigidbody->getAABB().min, rigidbody->getAABB().max}, glm::vec3(1, 0, 0)
        );
      }
    }*/

    // Prepare the next frame
    glfwSwapBuffers(window);
    glFinish();
  }

  void Game2D::quit() {
    // Destroy the window and terminate all OpenGL processes
    if (window) {
      glfwDestroyWindow(window);
      window = nullptr;
    }
    glfwTerminate();

    // Remove all the entities from the game
    for (const auto entity: entities)
      removeEntity(entity);
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

  void Game2D::addEntities() {
    for (auto entity: entitiesToAdd) {
      entities.push_back(entity);
      if (entity->texture)
        entitiesById[entity->texture->id].push_back(entity);
    }
    entitiesToAdd.clear();
  }

  void Game2D::addEntity(Entity2D *entity) {
    if (!entity || !instance || !instance->root || entity == instance->root)
      return;

    // Check if the entity is already in the list
    for (const auto e: instance->entities)
      if (*e == *entity)
        return;

    instance->entitiesToAdd.push_back(entity);
  }

  void Game2D::removeEntities() {
    for (auto entity: entitiesToRemove) {
      // If the entity has a texture, remove it from the draw list
      if (entity->texture) {
        const auto it2 = std::ranges::find_if(
          entitiesById[entity->texture->id],
          [&entity](const Entity2D *ptr) {
            return ptr == entity;
          }
        );
        if (it2 != entitiesById[entity->texture->id].end())
          entitiesById[entity->texture->id].erase(it2);
      }
      // Remove the entity from the game
      const auto it = std::ranges::find_if(
        entities,
        [&entity](const Entity2D *ptr) {
          return ptr == entity;
        }
      );
      if (it != entities.end()) {
        entities.erase(it);
        entity->quit();
      }
    }
    instance->entitiesToRemove.clear();
  }

  void Game2D::removeEntity(Entity2D *entity) {
    if (entity && instance && entity != instance->root && !instance->entities.empty())
      instance->entitiesToRemove.push_back(entity);
  }

  void Game2D::framebuffer_size_callback(GLFWwindow *window, const int, const int) {
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    instance->width = framebufferWidth;
    instance->height = framebufferHeight;

    const Vector2<size_t> initialSize = Engine::Settings::Window.GetScreenResolution();

    // Calculate the proper aspect ratio
    const float ratioX = static_cast<float>(framebufferWidth) / static_cast<float>(initialSize.x);
    const float ratioY = static_cast<float>(framebufferHeight) / static_cast<float>(initialSize.y);
    instance->aspectRatio = Vector2{ratioX, ratioY};

    const bool maintainAspectRatio = Engine::Settings::Graphics.GetMaintainAspectRatio();
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
