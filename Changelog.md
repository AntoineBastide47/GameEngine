# Changelog

All notable changes to this project will be documented in this file.<br>
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.2] - 2025-04-dd
### Added
- **[Animation2D]**: creates a horizontal or vertical animation from a sprite sheet
- **[Animator2D]**: manages a list of Animation2D and updates them accordingly
- **[AnimationSystem]**: updates all animators
### Changed
- **[Renderer2D, ParticleRenderer2D]**: better performance with correct use of GL_TRIANGLE_STRIPS and other memory dereferencing changes
### Fixed
- **[Instrumentor]**: profiling level condition is no longer inverted and made it a private class as it is an engine-only class

## [0.7.1] - 2025-04-24
### Added
- **[Timer]**: RAII timer to determine the running time of a function/scope
- **[Instrumentor]**: profiler that writes the timer data to a JSON file so that it can be displayed on [Chrome Tracer](chrome://tracing)
- **[Profiling]**: macros to run profiling on functions and scopes
### Changed
- **[ResourceManager]**: shaders are now single files
- **[engine-cli]**: the build command builds three static libraries: debug, debug with profiling and optionally release
- **[Templates/2D/CMakeLists.txt]**: links to the appropriate engine static library
- **[Physics2D]**: made broadPhase() more efficient thanks to profiler

## [0.7.0] - 2025-04-21
### Added
- **[Camera2D]**: follow target and camera shake
- **[Vector2]**: smoothDamp function
### Fixed
- **[Transform2D, ParticleRenderer2D]**: some sprites were not rendered when they should have been rendered

## [0.6.1] - 2025-04-20
### Added
- **[Camera2D]**: 2D orthographic camera
### Changed
- **[Renderer2D]**: now uses a map buffer range like the particle system
- **[Texture]**: id, width and height are now accessed with getters
- **[ResourceManager]**: auto detect image data for texture loading parameters
### Fixed
- **[Renderer2D]**: disabled static sprites still being rendered
- **[Game2D]**: frame rate limiting no longer causes jittering
### Removed
- **[Texture]**: all unnecessary texture loading variables

## [0.6.0] - 2025-04-17
### Added
- **[CMakeLists]**: MULTI_THREAD macro, used to enable and disable multi-threading, helpful for debugging
- **[Renderer2D]**: static entities have a different render pass from the non-static entities
- **[ResourceManager]**: sprites can be created from texture atlases 
### Changed
- **[engine-cli]**: looks for engine static library declaration instead of comment to determine if cli is in the engine or project directory
- **[Renderer2D]**: CPU sorts by shader and texture id and GPU sorts by render order (=depth)
- **[Game2D, ParticleSystemRenderer2D]**: multithreading code only compiled if MULTI_THREAD is set to 1
- **[ParticleSystem2D]**: uses a map buffer range instead of a float array for better performance
### Fixed
- **[ParticleSystem2D]**: particles no longer jitter 
### Removed
- **[Renderer2D]**: renderLayer as it is the same as render order currently

## [0.5.5] - 2025-04-10
### Added
- **[SpriteRenderer]**: sprites can be flipped on X and Y coordinates now
- **[Entity2D]**: framework for future static rendering
### Changed
- **[Transform2D]**: static entities' transform cannot be updated once the entity is initialized

## [0.5.4] - 2025-04-09
### Added
- **[Sprite]**: wrapper around Texture for rendering
- **[SpriteRenderer]**: component used to render entity sprites
### Changed
- **[Texture2D--->Texture]**: now a simple class to load a texture
- **[SpriteRenderer--->Renderer2D]**
### Removed
- **[Entity2D]**: no longer has texture and textureColor as member variables
- **[Game2D]**: entitiesToRender are now stored in the Renderer2D class

## [0.5.3] - 2025-04-03
### Added
- **[engine-cli]**: rebuild command rebuilds the cli's binary executable
### Changed
- **[Transform2D]**: Set and Update functions all call the main one
- **[engine-cli]**: commands must start with a "--" symbol now and multiple commands can be run together
### Fixed
- **[engine-cli]**: version command works correctly when in the Engine folder

## [0.5.2] - 2025-03-31
### Added
- **[CMakeLists]**: made most dependencies submodules
- **[engine-cli]**: simple binary that replaces the old makefile
### Changed
- **[CMakeLists]**: GLEW is now linked statically
- **[Templates/2D]**: templates use the same build command pattern as for the engine
- **[Collider2D]**: renamed RectangleCollider2D to BoxCollider2D
### Removed
- **[CMakeLists], [Templates/2D]**: link time optimization since it creates bugs in the release builds of the engine
- **[Project], [Templates/2D]**: make is no longer a dependency

## [0.5.1] - 2025-02-19
### Added
- **[Game2D]**: render thread handles rendering concurrently while the update loop executes
- **[Behaviour]**: base class for user defined components that has the callbacks Entity2D had
- **[ParticleSystemRenderer2D]**: mutex for data synchronization
### Changed
- **[Entity2D]**: no longer inheritable
### Removed
- **[Game2D]**: adding overrides of Entity2D
- **[Entity2D]**: virtual functions

## [0.5.0] - 2025-02-18
### Added
- **[ParticleSystem2D]**: more control and options over the particle system
- **[Vector2]**: glm extensions
### Fixed
- **[Physics2D]**: extra infinite impulse application removed
### Removed
- **[Vector2]**: custom vector classed changed in favour of glm vectors

## [0.4.3] - 2025-01-18
### Added
- **[SpriteRenderer]**: batching entities with the same texture into a single draw call
- **[ParticleSystem2D]**: batching all particles into a single draw call
### Changed
- **[Transform2D]**:
  - world matrix only recomputed when needed when the GetWorldMatrix function is called
  - position, rotation and scale are accessed/modified through helper functions Get, Set and Update
### Fixed
- **[Transform2D]**:
  - child entities now rotate correctly
  - entities being set to not visible when they were in fact visible
  - entities not conserving their world position, rotation and scale when parented to another entity
  - fixed textures being flipped on the X axis because of incorrect scaling in the projectionMatrix
- **[Physics2D]**:
  - child collider will only collide with parent if both have a rigidbody, rotations would cause problems in colliders
  - more than one impulse is applied when colliders stay in collision
### Removed
- **[Property]**: used much more memory than needed, replaced by getters, setters and updaters
- **[Settings]**: frame skipping as it will become unmanageable as the engine grows, if a good solution is found later on it might be brought back

## [0.4.2] - 2025-01-17
### Added
- **[ParticleSystem2D]**: component to render particles to the screen
- **[ParticleSystemRenderer2D]**: renders all the particle systems and contains the render data, allows for less memory usage
- **[Property]**: added a getter
- **[Component2D]**: added the following messages OnInitialize, OnUpdate, OnFixedUpdate, OnDestroy
- **[Game2D]**: particle shader, particle rendering and calling of component messages
### Changed
- **[Rigidbody2D]**: changed step to use OnFixedUpdate
### Removed
- **[Physics2D]**: no more tracking of rigidbodies
- **[Entity2D]**: initialized flag

## [0.4.1] - 2025-01-13
### Added
- **[Engine]**: new Engine folder that contains the header files and templates of the engine. The create-project command has been moved to the makefile contained in the folder.
- **[makefile]**: `build-debug` and `build-release` for the build pipeline that set the build type of the engine. When using build-release, a zip file is generated with all the necessary files for distribution.
- **[Game2D]**: `limitFrameRate` and `resetFlags` to declutter the body of the run function
- **[makefile]**: platform check to make sure the engine is only running on macOS, Linux or Windows
- **[Transform2D]**: conversion from world coords to local coords
- **[Collider2D]**: new flag `draw` allows the visualization of collider bounds in debug builds
### Changed
- **[CMakeLists]**: the static library is now a dynamic library, makes building the game faster and removes unnecessary imports in the game's CMakeLists file
- **[Log]**: messages, warnings and errors are sent only once to declutter the console
### Fixed
- **[Game2D]**: window not re-rendering on move/resize when enabling frame skipping
### Removed
- **[Game2D]**: collider drawing
- **[makefile]**: removed build-clean and create-project

## [0.4.0] - 2025-01-07
Finished the first version of the physics engine.
### Added
- **[Rigidbody2D]**: isKinematic flag which allows the physics engine to apply forces to the body
- **[Collider2D]**: added flag autoCompute to allow the engine to automatically compute the collider, if disabled, the user must pass in all the position, width, height and radius information to the collider
### Changed
- **[Rigidbody2D]**: changed isStatic to isKinematic, made the velocity public
- **[Game2D, Entity2D]**: changes message naming convention, message callbacks now all start with the prefix "On"
- **[Transform2D]**: added preCallback to rotation to clamp it between 0 and 360
- **[Property]**: changed type of preCallback to T(T newValue), it is now used to process the newValue before setting it
### Fixed
- **[Settings]**: fixed SetTargetFrameRate not working properly when setting it to 0
- **[Transform2D]**: fixed projectionMatrix incorrectly using the world rotation
### Removed
- **[Rigidbody2D**]: removed viewport binding
