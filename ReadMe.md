# Game Engine  [![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](https://github.com/AntoineBastide47/GameEngine/blob/main/LICENSE)
This game engine is a personal project I'm working on to learn more about game development and C++. It is currently
designed to run 2D games and aims to provide a simple yet powerful framework for building interactive applications.

## Getting Started
### 0. Prerequisites
This project requires [cmake](https://cmake.org), [GLEW](https://github.com/nigels-com/glew?tab=readme-ov-file#build), [clang](https://clang.llvm.org) and [llvm](https://llvm.org) to be installed on your system for proper functionality.
- **macOS:**
```bash
brew install cmake glew llvm
echo 'export PATH="/usr/local/opt/llvm/bin:$PATH"' >> ~/.bash_profile
export PATH="/usr/local/opt/llvm/bin:$PATH"
```
- **Linux:**
```bash
sudo apt-get update
sudo apt-get install build-essential libgl1-mesa-dev libglew-dev clang llvm
sudo snap install cmake --classic
```
- **Windows:**
Manual installation required, follow the links above to do so.
### 1. Clone the Repository:
```bash
git clone --recursive https://github.com/AntoineBastide47/GameEngine.git
git submodule update --init --recursive
```
### 2. Build the CLI
Build the command-line interface (CLI) to manage the engine and projects:
```bash
cd ./tools/CLI
cmake -B build
cmake --build build
cd ../..
```
Run the CLI with:
```bash
./engine-cli
```
### 3. Build the engine tools
Build the engine tools to make sure all it's features work correctly:
```bash
./engine-cli --build-tools
```
### 4. Build the engine and editor
#### 4.1 Building the engine, the SDK and the editor
```bash
./engine-cli --build
```
_Tip: Add the argument `true` after the build command to also build the `release mode` library for production builds._
#### 4.2 Building the engine libraries only
```bash
./engine-cli --build-engine
```
_Tip: Add the argument `true` after the build command to also build the `release mode` library for production builds._
#### 4.3 Building the editor executable only
```bash
./engine-cli --build-editor
```
_Tip: Add the argument `true` after the build command to also build the `release mode` executable for production builds._
### 4.4 Building the SDK
```bash
./engine-cli --build-sdk
```
### 5. Distributing the Engine
1. Build the engine with all configurations:
```bash
./engine-cli --build true
```
2. Compress the `SDK/` folder:
```bash
zip -r EngineSDK.zip SDK
```
4. Rename the zip file if desired, then distribute it.
### 6. Build everything at once
```bash
cd ./tools/CLI && cmake -B build && cmake --build build && cd ../..
./engine-cli --build-tools --build true --build-sdk
```
### 7. CLI command information
For a list of available commands:
```bash
./engine-cli
```
or
```bash
./engine-cli --help
```

## Using the engine 
### 1. Create a project
Make sure the whole engine is built, then open the SDK folder in the terminal and run:
```bash
./engine-cli --create-project
```
### 2. Running a Game Project
1. Navigate to your game folder:
```bash
cd your/game/folder/location
```
2. Build and run the game
```bash
./engine-cli --build-project debug --run
```
_Tip: Replace `debug` with `release` for production builds._
### 3. Profiling
To profile your project:
1. Build and run with profiling enabled:
```bash
./engine-cli --build-project profile --run
```
2. After the run completes, a file named `profiler` will be generated in the root directory of your project.
3. Manually open Chrome and navigate to `chrome://tracing` by typing it in the address bar.
4. Drag and drop the `profiler` file onto the tracing window to view the profiling results.

## Project Structure
```
GameEngine/
├─ Editor/            # Editor GUI application
│  └─ (include|src)/  # Editor source code (headers|source) respectively
├─ Engine/            # Engine source code and CMake config
│  ├─ Assests/        # Empty but required for CMRC to work
│  └─ (include|src)/  # Engine source code (headers|source) respectively
│     ├─ Engine/      # All engine code non-specific to 2D
│     ├─ Engine2D/    # All engine code specific to 2D
│     └─ Shaders/     # Engine defined shaders (only in the include directory)
├─ SDK/               # Generated SDK for engine distribution
├─ Templates/         # Project templates
│  └─ 2D/             # Template to create a brand new 2D game
├─ tools/             # CLI and internal development tools
│  ├─ CLI/            # engine-cli source code
│  └─ HeaderForge/    # header-forge source code
├─ vendor/            # Third-party libraries
├─ CMakeLists.txt     # Top-level build configuration
├─ engine-cli         # CLI tool to manage the engine and projects
└─ header-forge       # Tool for header parsing to generate reflection code
```

## Current Features
* **Rendering Engine:** Efficient rendering of sprites and textures.
* **Entity Management:** Handles entities and their components.
* **Input Handling:** Supports keyboard, mouse, and gamepad input.
* **Physics Engine:** Provides rigid body dynamics and collision responses.
* **CPU based Particle System:** Configurable particle emitters with control over lifetime, velocity, and color transitions.
* **Simple Profiler:** Captures timestamped CPU events and exports them to a trace file compatible with Chrome’s chrome://tracing viewer for performance analysis.
* **2D Camera System:** orthographic camera and follow-camera modes with zoom, pan, rotation and shake support.
* **Animator:** Data-driven 2D animation system supporting transitions, conditions, triggers, and parameterized state machines.
* **JSON Library:** JSON library mirroring python/javascript json usages and a very fast DOM parser
* **Scene Management**: Create runtime scenes, Save scenes to json files and Load scenes from json files 

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
Releases will begin with version 1.0.0 and will be available for Windows, macOS, and Linux.

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
> The engine is not backwards compatible to avoid increased binary size and potential performance issues.

## Limitations
> [!WARNING]
> The engine is primarily developed on macOS. Linux and Windows support may be limited until version `1.0.0` is released.

## Contributing
Contributions are welcome. As this is a personal learning project, contributions may not be merged immediately. Fork the repository and experiment freely.

## Learning Objectives
* **C++ Mastery:** Improve modern C++ skills.
* **Game Development:** Learn the fundamentals of game engine design.
* **Software Architecture:** Apply design patterns and architectural principles.
* **Project Management:** Gain experience in planning, versioning, and documentation.

## License
This project is licensed under the Apache License Version 2.0.

## References
The following references include a collection of articles and videos that have served as the foundation for building my
engine (the list will evolve over time):
- OpenGL, Particle System: [Learn OpenGL](https://learnopengl.com/)
- Physics Engine: [Two-Bit Coding - Let's Make a Physics Engine](https://www.youtube.com/playlist?list=PLSlpr6o9vURwq3oxVZSimY8iC-cdd3kIs)
- Profiler: [The Cherno - Game Engine](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
> [!Important]
> These resources are guides. They provide insights but may not result in an identical engine implementation.