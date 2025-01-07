//
// Game2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef GAME2D_H
#define GAME2D_H

#include <functional>
#include <string>
#include <unordered_set>
#include <cmrc/cmrc.hpp>

#include "2D/Entity2D.h"
#include "2D/Physics/Physics2D.h"
#include "2D/Types/Vector2.h"
#include "Common/RenderingHeaders.h"
#include "Input/InputContexts.h"

using ResourceLoader = std::function<cmrc::file(const std::string &)>;

namespace Engine {
  class Settings;
}

namespace Engine2D::Physics {
  class Collider2D;
}

namespace Engine2D {
  /** Game2D is the class that represents a game and manages each part of it. */
  class Game2D {
    friend class Entity2D;
    friend class ResourceManager;
    friend class Transform2D;
    friend class Physics::Collider2D;
    friend class Physics::Physics2D;
    friend class Engine::Settings;
    public:
      /// @returns The width of the viewport of the window
      [[nodiscard]] static float ViewportWidth();
      /// @returns The height of the viewport of the window
      [[nodiscard]] static float ViewportHeight();
      /// @returns The aspect ratio of the viewport
      [[nodiscard]] static Vector2f AspectRatio();
      /// @returns The time span between the current frame and the last frame
      [[nodiscard]] static float DeltaTime();
      /// @returns The fixed time span between the physics updates
      [[nodiscard]] static float FixedDeltaTime();
      /// @returns The entity with the given name if it was found, nullptr if not
      template<typename T> requires std::is_base_of_v<Entity2D, T>
      static std::shared_ptr<T> Find(const std::string &name) {
        for (const auto entity: instance->entitiesToAdd)
          if (entity->name == name)
            return std::static_pointer_cast<T>(entity);
        for (const auto entity: instance->entities)
          if (entity->name == name)
            return std::static_pointer_cast<T>(entity);
        return nullptr;
      }

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
      /// Creates an entity of the given type and adds it to the game
      template<typename T> requires std::is_base_of_v<Entity2D, T>
      static std::shared_ptr<T> AddEntity(std::string name = "Entity") {
        if (instance) {
          std::shared_ptr<T> entity = std::make_shared<T>(name);
          instance->entitiesToAdd.insert(entity);
          return entity;
        }
        return nullptr;
      }
    protected:
      /**
       * Creates a game
       * @param width The width of the game window
       * @param height The height of the game window
       * @param title The title of the game window
       */
      Game2D(int width, int height, std::string title);
      virtual ~Game2D() = default;

      /// Called during initialization, allowing derived classes to customize behavior.
      virtual void OnInitialize() {}
    private:
      /// Used to resize the coordinate space. If initial window is 1920x1080 and factor is 0.1f, the coordinate space will
      /// be 192x108.
      static const float screenScaleFactor;
      /// The current aspect ratio of the window
      Vector2f aspectRatio;

      /// The title of the game window
      std::string title;
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
      std::unordered_set<std::shared_ptr<Entity2D>> entities;
      /// Entities scheduled to be added to the game
      std::unordered_set<std::shared_ptr<Entity2D>> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::unordered_set<std::shared_ptr<Entity2D>> entitiesToRemove;
      /// The entities of the game grouped by there texture id
      std::map<int, std::unordered_set<std::shared_ptr<Entity2D>>> entitiesToRender;

      /// The time during two frames
      float deltaTime;
      /// The time scaling factor
      float timeScale;
      /// The frame rate targeted by the engine
      float targetFrameRate;
      /// The rate at which the game should update
      float targetRenderRate;
      /// If the new frame needs rendering
      bool currentFrameNeedsRendering;

      /// Physics simulator
      Physics2D *physics2D;
      /// Timer used to check if the game is ready for the next physics update
      float physicsAccumulator;

      /// Initializes the game
      void initialize();
      /// Processes all the inputs to the game
      static void processInput();
      /// Update the game
      void update();
      /// Simulates a step of the physics simulation
      void fixedUpdate();
      /// Renders the game
      void render() const;
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
      static void window_refresh_callback(GLFWwindow *window);
      static void window_pos_callback(GLFWwindow *window, int x, int y);
      static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
  };
} // Engine2D

#endif //GAME2D_H
