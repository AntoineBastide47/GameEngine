# Game Engine  [![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](https://github.com/AntoineBastide47/GameEngine/blob/main/LICENSE)
This game engine is a personal project I'm working on to learn more about game development and C++. It is currently
designed to run 2D games and aims to provide a simple yet powerful framework for building interactive applications.

## Getting Started
### 0. Prerequisites
This project requires [cmake](https://cmake.org) and [GLEW](https://github.com/nigels-com/glew?tab=readme-ov-file#build) to be installed on your system for proper functionality.
- **macOS:**
```bash
brew install cmake glew
```
- **Linux:**
```bash
sudo apt-get update
sudo apt-get install build-essential libgl1-mesa-dev libglew-dev
sudo snap install cmake --classic
```
- **Windows:**
Manual installation required, follow the links above to do so.
### 1. Clone the Repository:
```bash
git clone --recursive -j8 https://github.com/AntoineBastide47/GameEngine.git
cd GameEngine
```
### 2. Build the CLI
Build the command-line interface (CLI) to manage the engine and projects:
```bash
cd CLI
cmake -B build
cmake --build build
```
Run the CLI with:
```bash
./engine-cli
```
### 3. Build the engine
- **Debug Mode** (recommended for development; provides detailed error messages):
```bash
./engine-cli build debug
```
- **Release Mode** (recommended for production; optimized performance):
```bash
./engine-cli build release
```
### 4. Create a project
1. Open the folder `GameEngine` in your terminal
2. Run the CLI command:
```bash
./engine-cli create-project
```
### 5. Running a Game Project
1. Navigate to your game folder:
```bash
cd your/game/folder/location
```
2. Build and run the game
```bash
./engine-cli build-project debug
./engine-cli run
```
_Tip: Replace `debug` with `release` for production builds._
### 6. CLI command information
For a list of available commands:
```bash
./engine-cli
```
or
```bash
./engine-cli help
```

## Current Features
- **Rendering Engine:** Efficient rendering of sprites and textures.<br>
- **Entity Management:** Handles entities and their components.<br>
- **Input Handling:** Supports keyboard, mouse, and gamepad input.<br>
- **Physics Engine:** Provides rigid body dynamics and collision responses.<br>

> [!NOTE]
> For planned updates and upcoming features, see the
> [Trello Board](https://trello.com/invite/b/67376d9fff131de8914e5da1/ATTI005d420929932a037057431249289ba6283CFF05/game-engine)

## Examples
Check out simple example games using the engine here: [Test Games](https://github.com/AntoineBastide47/TestGames)
> [!WARNING]
> The game code is subject to be completely rewritten as the engine develops.

## Versioning
Versions are currently formatted as follows: `[major].[feature].[patch].[beta]` (e.g., `1.2.3`)<br>
- **Major Version:** Significant updates or breaking changes.
- **Feature Count:** Incremented with each new fully implemented feature or when enough smaller patches were made.
- **Patch Version:** Minor updates, bug fixes or small additions.
- **Beta Version:** Preliminary releases that may be unstable. Read the commit description before using.

**Versioning Examples**
- `0.1.0:` Game loop and texture rendering.
- `0.1.1:` Keyboard Input System.
- `0.2.0:` Input System.
- `0.4.0.1:` ReadMe update
- `1.0.0:` First release of the engine.

## Commit Guidelines
- **Title:** Brief description (e.g., "Version 0.5.2").
- **Description:** Overview of changes made.

For detailed commit history, see the [Changelog.md](https://github.com/AntoineBastide47/GameEngine/blob/main/Changelog.md).

## Releases
Releases begin with version 1.0.0 and are available for Windows, macOS, and Linux. Future releases (version X.1.0) will focus on performance and memory optimizations.

## Modifying the engine
Feel free to fork and modify the engine. After making changes:
1. Rebuild the engine.
2. Rebuild and run your game to link the updated engine.

## API Notices
> [!IMPORTANT]
> - Naming conventions are not standardized yet; expect changes.
> - APIs may change significantly as the engine evolves.
> - The build pipeline may be updated, so check the changelog for major changes.

> [!WARNING]
> The engine is not backwards compatible to avoid increased binary size and reduced performance.

## Limitations
> [!WARNING]
> The engine is primarily developed on macOS. Linux and Windows support may be limited until version `1.0.0` is released.

## Contributing
Contributions are welcome. As this is a personal learning project, contributions may not be merged immediately. Fork the repository and experiment freely.

## Learning Objectives
**C++ Mastery:** Improve modern C++ skills.<br>
**Game Development:** Learn the fundamentals of game engine design.<br>
**Software Architecture:** Apply design patterns and architectural principles.<br>
**Project Management:** Gain experience in planning, versioning, and documentation.

## License
This project is licensed under the Apache License Version 2.0.

## References
The following references include a collection of articles and videos that have served as the foundation for building my
engine (the list will evolve over time):
- OpenGL, Particle System, Audio: [Learn OpenGL](https://learnopengl.com/)
- Physics Engine: [Two-Bit Coding - Let's Make a Physics Engine](https://www.youtube.com/playlist?list=PLSlpr6o9vURwq3oxVZSimY8iC-cdd3kIs)<br>
> [!Important]
> These resources are guides. They provide insights but may not result in an identical engine implementation.