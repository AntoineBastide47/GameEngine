//
// Game2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "2D/Game2D.h"

#include <iostream>
#include <thread>
#include <__algorithm/ranges_find_if.h>

#include "2D/ResourceManager.h"
#include "Common/Macros.h"
#include "Common/RenderingHeaders.h"
#include "Input/Gamepad.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace Engine2D {
  Game2D *Game2D::instance = nullptr;

  Game2D::Game2D(const int width, const int height, std::string title)
    : width(width), height(height), title(std::move(title)), window(nullptr), deltaTime(0.0f), root(new Entity2D("Root")),
      spriteRenderer(nullptr), frameRate(0.0f) {
    if (instance)
      throw std::runtime_error("ERROR::GAME2D: There can only be one instance of Game2D running.");
    if (width <= 0 || height <= 0)
      throw std::invalid_argument("ERROR::GAME2D: Game window size must be greater than zero");
    instance = this;
  }

  int Game2D::Width() {
    return instance->width;
  }

  int Game2D::Height() {
    return instance->height;
  }

  float Game2D::DeltaTime() {
    return instance->deltaTime;
  }

  Game2D *Game2D::Instance() {
    return instance;
  }

  void Game2D::Run() {
    this->initialize();

    const float targetFrameTime = this->frameRate == 0.0f ? 0.0f : 1.0f / this->frameRate;

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

      // Update the game
      this->update();

      // Render the game
      glClearColor(0, 0, 0, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      this->render();

      glfwSwapBuffers(window);

      // Calculate frame time and add a delay if the frame was too fast
      auto frameEndTime = std::chrono::high_resolution_clock::now();
      if (const float frameDuration = std::chrono::duration<float>(frameEndTime - currentFrameTime).count();
        frameDuration < targetFrameTime) {
        std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime - frameDuration));
      }
    }

    this->quit();
  }

  void Game2D::initialize() {
    // Initialize and Configure GLFW
    if (!glfwInit()) {
      std::cerr << "ERROR::GAME2D: GLFW could not initialize" << std::endl;
      this->quit();
      exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create the window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
      std::cerr << "ERROR::GAME2D: Failed to create window" << std::endl;
      this->quit();
      exit(EXIT_FAILURE);
    }

    // Set window callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glViewport(0, 0, width, height);
    glfwMakeContextCurrent(window);

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
      std::cerr << "ERROR::GAME2D: Failed to initialize GLEW: " << err << std::endl;
      this->quit();
      exit(EXIT_FAILURE);
    }

    // Initialize input system
    Engine::Input::Keyboard::initialize(this->window);
    Engine::Input::Mouse::initialize(this->window);

    // Create and configure the sprite renderer
    ResourceManager::LoadShader("EngineFiles/Shaders/sprite.vs", "EngineFiles/Shaders/sprite.fs", "", "sprite");
    const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite")->SetInteger("sprite", true);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection, true);
    spriteRenderer = new Rendering::SpriteRenderer(ResourceManager::GetShader("sprite"));

    this->Initialize();
  }

  void Game2D::update() {
    this->Update();
    for (const auto entity: entities)
      if (entity->active)
        entity->update();
  }

  void Game2D::render() const {
    for (const auto entity: entities)
      if (entity->active && entity->texture)
        spriteRenderer->DrawSprite(entity);
  }

  void Game2D::quit() {
    this->Quit();
    // Deallocate all the entity pointers
    for (const auto entity: entities)
      RemoveEntity(entity);
    entities.clear();
    ResourceManager::Clear();
    SAFE_DELETE(spriteRenderer);
    // Destroy the window and terminate all OpenGL processes
    if (window) {
      glfwDestroyWindow(window);
      window = nullptr;
    }
    glfwTerminate();
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

  void Game2D::AddEntity(Entity2D *entity) {
    if (!entity || !instance || entity == instance->root)
      return;

    // Check if the entity is already in the list
    for (const auto e: instance->entitiesSearch)
      if (*e == *entity)
        return;

    instance->entities.push_back(entity);
    instance->entitiesSearch.insert(entity);
  }

  void Game2D::RemoveEntity(Entity2D *entity) {
    if (!entity || !instance || entity == instance->root || instance->entities.empty())
      return;
    if (const auto itSearch = instance->entitiesSearch.find(entity); itSearch != instance->entitiesSearch.end()) {
      const auto it = std::ranges::find_if(instance->entities,
                                           [&entity](const Entity2D *ptr) {
                                             return ptr == entity;
                                           }
                                          );
      if (it != instance->entities.end()) {
        instance->entities.erase(it);
        instance->entitiesSearch.erase(itSearch);
        entity->Quit();
      }
    }
  }

  void Game2D::framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
  }

  void Game2D::scroll_callback(GLFWwindow *window, double xOffset, const double yOffset) {
    Engine::Input::Mouse::processScroll(yOffset);
  }
}
