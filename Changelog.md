# Changelog

All notable changes to this project will be documented in this file.<br>
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
  - more than one impulse applied when colliders stay in collision
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
- **[CMakeLists]**: static library is now a dynamic library, makes building the game faster and removes unnecessary imports in the game's CMakeLists file
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
- **[Collider2D]**: added flag autoCompute to allow the engine to automatically compute the collider, if disabled the user must pass in all the position, width, height and radius information to the collider
### Changed
- **[Rigidbody2D]**: changed isStatic to isKinematic, made the velocities public
- **[Game2D, Entity2D]**: changes message naming convention, message callbacks now all start with the prefix "On"
- **[Transform2D]**: added preCallback to rotation to clamp it between 0 and 360
- **[Property]**: changed type of preCallback to T(T newValue), it is now used to process the newValue before setting it
### Fixed
- **[Settings]**: fixed SetTargetFrameRate not working properly when setting it to 0
- **[Transform2D]**: fixed projectionMatrix incorrectly using the world rotation
### Removed
- **[Rigidbody2D**]: removed viewport binding
