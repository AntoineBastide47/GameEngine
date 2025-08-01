# Changelog

All notable changes to this project will be documented in this file.<br>
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.10.1] - 2025-08-01
### Added
- **[[Shader](include/Engine/Rendering/Shader.hpp), [Texture](include/Engine/Rendering/Texture.hpp)]**: both store their file path for serialization and deserialization
- **[[CLI/BuildSDK](tools/CLI/include/commands/BuildSdk.hpp)]**: builds an SDK folder with all the required files and folders for releases
### Fixed
- **[[SceneManager](include/Engine2D/SceneManagement/SceneManager.hpp)]**: scene loading in multi threaded mode now works by offsetting the creation of gpu shader and textures to the render thread

## [0.10.0] - 2025-07-31
### Added
- **[[Reflectable](include/Engine/Reflection/Reflectable.hpp)]**: OnSerialize and OnDeserialize callbacks are used to perform custom logic during serialization and deserialization, allowing types to handle state beyond automatic member reflection
- **[[ResourceManager](include/Engine/ResourceManager.hpp)]**: functions to get the name of their associated resource
- **[[Behaviour](include/Engine2D/Behaviour.hpp)]**: OnBindInput callback specifically used for input binding, called after OnInitialize and OnDeserialize
- **[[Entity2D](include/Engine2D/Entity2D.hpp)]**:
  - AddInputCallback for automatic input callback removal when the entity is destroyed
  - Instantiate to create an entity, removed from Game2D
  - GetComponent(s)InChildren to find components in the children entities
  - TryGetComponent to check if an entity has a component of the given type and if so use it without having to call HasComponent and GetComponent
- **[[Scene](include/Engine2D/SceneManagement/Scene.hpp)]**:
  - SetMainCamera to change the main camera of the scene
  - Find, FindObjectOfType and FindObjectsOfType to find entities by name or components in entities
- **[[SceneResources](include/Engine2D/SceneManagement/SceneResources.hpp)]**: saves and loads all the scene resources when scenes are saved and loaded (shaders, textures, sprites)
- **[[Transform2D](include/Engine2D/Transform2D.hpp)]**: MakeNthChild to change the ordering of children entities
### Changed
- **[[Save](include/Engine/Reflection/Save.hpp), [Load](include/Engine/Reflection/Load.hpp)]**: containers use resize and reserve calls wherever possible to make loading and saving as fast as possible
### Fixed
- **[[ReflectionFactory](include/Engine/Reflection/ReflectionFactory.hpp)]**: CreateUnique now returns a unique pointer instead of a shared pointer

## [0.9.3] - 2025-07-27
### Added
- **[[Scene](include/Engine2D/SceneManagement/Scene.hpp)]**: object that contains all the entities and everything needed to update, render, animate, ... them for the game to work
- **[[SceneManager](include/Engine2D/SceneManagement/SceneManager.hpp)]**: static class used to create, load, switch and unload scenes
### Changes
- Made all the core systems (physics, animation, renderer) non-static and scene bound
- **[[Physics2D](include/Engine2D/Physics/Physics2D.hpp)]**: friction uses geometric mean for static and dynamic coefficients
### Fixed
- **[[HeaderForge](tools/HeaderForge)]**:
  - the SERIALIZE_* macro not being included or being added in enum definitions
  - the clang error logged in the terminal: "error: invalid argument '-std=c++20' not allowed with 'C'"
- **[[Collisions](include/Engine2D/Physics/Collisions.hpp)]**:
  - circle-circle collisions now work as expected
  - polygon-polygon collisions now work as expected

## [0.9.2] - 2025-07-25
### Changed
- **[Engine2D/Physics/Physics2D]**:
  - clamped linear and angular velocity to prevent infinite movement nad/or rotation speed
  - fixed multi body collision increasing speed
  - fixed friction incorrectly increasing/decreasing speed
### Removed
- **[Engine2D/Settings]**: removed the friction enabled setting, friction is now per object 

## [0.9.1] - 2025-07-23
### Changed
Game2D.hpp and Entity2D.hpp now hold unique pointers for entities and components, while all the systems hold raw pointers
for clear data ownership to prepare for scene saving. So now, entity and component APIs return raw pointers instead of
shared pointers to prevent any dangling references.

## [0.9.0] - 2025-07-20
### Added
- **[Engine/Data/JSON]**: new Get functions for each json type: GetNull, GetBoolean, GetNumber, GetArray and GetObject
- **[Engine/Reflection/Save]**: save functions for enums, glm::vec, glm::mat and std::variant
- **[Engine/Reflection/Load]**: load functions to mirror all the save functions
- **[Engine/Reflection/Serializer]**: ToJsonString and ToJsonToFile to convert to a stringified json or, to convert to json and save to a file
- **[Engine/Reflection/Deserializer]**: FromJson, FromJsonString and FromJsonFromFile to convert json to types 
- **[Engine/Reflection/ReflectionFactory]**: factory to create shared and unique pointers of records that inherit from Engine::Reflection::Reflectable from their fully qualified string name
### Changed
- **[tools/CLI]**:
  - made the build process faster by using all available CPU cores and using Ninja if it is detected on the system
  - when running the link command, the header-forge binary is also sent to the game folder
- **[Engine/Data/JSON]**: constructors accepts more types with templating
- **[tools/HeaderForge]**: generates load functions, reflection factory code and, inject an include statement and the SERIALIZE_* macro into the .hpp files
- **[Engine/Reflection/Save]**: save function create a JSON instance instead of building JSON strings
### Removed
- **[Engine/Data/JSON]**: Get<T> function

## [0.8.6] - 2025-07-16
### Changed
- **[Engine/Data/JSONParser]**: changes the internal string parsing to use the istream parsing with a zero copy buffer, made the parsing ~50% faster

## [0.8.5] - 2025-07-12
### Added
- **[Engine/Data/JSON]**: simple JSON library
- **[Engine/Data/JSONParser]**: JSON parser that parses JSON from either strings or istream's

## [0.8.4] - 2025-06-21
### Added
- **[Macros/Utils]**: added macros for fully qualified class name
### Changed
- **[Save.hpp]**: moved all save functions in Serializer.hpp to a separate file
- **[Serializer]**: only contains the To* functions
- **[Reflectable]**: renamed from Serializable, contains the ENGINE_SERIALIZE and ENGINE_NON_SERIALIZABLE macros
- **[HeaderForge/Generator]**: renamed from Injector, now procedurally creates SERIALIZE_${CLASS_NAME} macros that override function definitions in Reflectable so that the classes and pointers can be saved correctly
### Removed
- **[Macros/Serialization]**
- **[Reflectable]**: TypeID() and StaticTypeID()
- **[HeaderForge/Parser]**: template usages

## [0.8.3] - 2025-06-21
### Added
- **[HeaderForge/Concepts]**: moved all concepts from HeaderForge/Parser to a separate file for readability
- **[HeaderForge/Parser]**: template parameters and fields from base classes/structs to Record
- **[HeaderForge/Serializer]**: convert some classes from the STL and user classes marked as serializable to JSON
- **[HeaderForge/Serializable]**: base class to allow the serializer to correctly serialize polymorphic classes
- **[HeaderForge/Injector]**: once files have been parsed, adds save and load declaration for user classes in the Serializer and Deserializer
### Changed
- **[Macros]**: made internal engine macros start with _e_ to "hide" them

## [0.8.2] - 2025-06-17
### Added
- **[.clang-format]**: contains the formatting used for hpp and cpp files by the engine
- **[Macros/Serialization]**: macros used to tell the parser which records, fields and pointers can be used for serialization
- **[CLI]**: added the command build-tools
- **[HeaderForge]**: start of the serialization engine with the parser
- **[HeaderForge/Parser]**: reads header files and extracts the serializable data using clang
### Changed
- **[CLI]**: moved to tools/CLI

## [0.8.1] - 2025-05-22
### Added
- **[ShaderPreProcessor]**: support for custom #define, #ifdef, #ifndef, #else, #endif statements
### Changed
- **[Event]**: += operator and AddCallback return a handle to remove the given callback easily
### Fixed
- **[Entity2D]**: destroy not calling the OnDestroy method of it's behaviours
- **[Event]**: remove callback not actually removing the callback

## [0.8.0] - 2025-05-22
### Added
- **[Animator2D]**: animator transitions that switch between different animations automatically based on user defined conditions
### Changed
- **[ResourceManager]**: transparent sprites are auto-detected and no longer need to be manually specified
### Fixed
- **[Entity2D]**: adding a Component2D would cause errors due to wrong casts to Behaviour
- **[sprite.glsl]**: flip not being applied correctly
- **[Log]**: no longer shows calls to internal logging in the stack trace

## [0.7.5] - 2025-05-12
### Added
- **[ShadersPreProcessor]**: script that updates the shaders depending on the define statements they contain
- **[Shaders]**: Matrices and Textures define statements that insert view/projection matrices and texture sampling in the shader
- **[Renderable2D]**: base class for SpriteRenderer and ParticleSystem2D
- **[ParticleSystem2D]**: blend modes for different render styles
### Changed
- **[Templates/2D]**: copies the shaders into the build directory and does not embed .DS_Store files
- **[Camera2D]**: updates the shader matrices once per frame instead of once per frame per shader
- **[ParticleSystem2D]**: combined the update and render functions into a single function
- **[ParticleSystemRenderer2D]**: renamed to ParticleSystemRegistry2D
- **[Renderer2D]**: now also renders particles, opaque non-static rendering is more performant
### Fixed
- **[Renderer2D]**: fixed transparent sprites and particles not respecting render order correctly
### Removed
- **[particle.glsl]**

## [0.7.4] - 2025-04-28
### Added
- **[Templates/2D]**: added a .gitignore
### Changed
- **[ParticleSystemRenderer]**: split rendering into opaque and transparent
- **[Camera2D]**: removed default shake values and split the coefficient array into X and Y variables
### Fixed
- **[Renderer2D]**: transparent sprites not being able to be rendered behind opaque sprites
- **[ParticleSystemRenderer]**: transparent particles not rendering correctly in front of transparent entities

## [0.7.3] - 2025-04-27
### Added
- **[Sprite]**: transparent flag
### Changed
- **[ResourceManager]**: when loading or creating a sprite the transparent flag is needed, and rect optional
- **[Renderer2D]**: split rendering into opaque and transparent
### Fixed
- **[Renderer2D]**: particles not rendering correctly under transparent sprites

## [0.7.2] - 2025-04-27
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
