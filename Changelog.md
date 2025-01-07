# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.4.0] - 2025-01-07

### Added
- [Rigidbody2D]: isKinematic flag which allows the physics engine to apply forces to the body
- [Collider2D]: added flag autoCompute to allow the engine to automatically compute the collider, if disabled the user must pass in all the position, width, height and radius information to the collider

### Changed
- [Rigidbody2D]: changed isStatic to isKinematic, made the velocities public
- [Game2D, Entity2D]: changes message naming convention, message callbacks now all start with the prefix "On"
- [Transform2D]: added preCallback to rotation to clamp it between 0 and 360
- [Property]: changed type of preCallback to T(T newValue), it is now used to process the newValue before setting it

### Fixed
- [Settings]: fixed SetTargetFrameRate not working properly when setting it to 0
- [Transform2D]: fixed projectionMatrix incorrectly using the world rotation

### Removed
- [Rigidbody2D]: removed viewport binding
