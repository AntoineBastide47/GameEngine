# Game Engine  [![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](https://github.com/AntoineBastide47/GameEngine/blob/main/LICENSE)
This game engine is a personal project I'm working on to learn more about game development and C++. It is currently
designed to run 2D games and aims to provide a simple yet powerful framework for building interactive applications.
The project is both a fun endeavor and an educational journey into the world of game engine architecture.

## Getting Started
To get started with the engine:

### 1. Clone the Repository:
```bash
git clone --recursive -j8 https://github.com/AntoineBastide47/GameEngine.git
```
### 2. Build the engine
#### 2.1. Build the engine in debug mode if you want readable error messages
```bash
make build-debug
```
#### 2.2. Build the engine in release mode if you want better performance, will make debugging errors harder
```bash
make build-release
```
### 3. Create a project
#### 3.1. Open the folder `GameEngine/Engine` in your terminal
#### 3.2. Run the shell CLI
```bash
make create-project
```
### 4. Running the newly created game project
#### 4.1. Open the location of the game folder in your terminal
```bash
cd your/game/folder/location
```
#### 4.2. Build and run the game
```bash
make build run
```
### 5. Make command information
```bash
make
```
or
```bash
make help
```
These two commands will give you descriptions for each available make command and are available in every folder where
you previously ran a `make` command in the terminal

## Current Features
**Rendering Engine:** Efficient rendering of sprites and textures.<br>
**Entity Management:** Managing the lifetime of entities and their components.<br>
**Input Handling:** Keyboard, mouse and gamepad input.<br>
**Physics Engine:** Rigidbodies and collision responses.<br>

> [!NOTE]
> To check out what I plan to add to the engine in the future, or what I am currently working on, check out this link:
> [Trello Board](https://trello.com/invite/b/67376d9fff131de8914e5da1/ATTI005d420929932a037057431249289ba6283CFF05/game-engine)

## Examples
For example games that implement the engine, check out: [Test Games](https://github.com/AntoineBastide47/TestGames)
> [!WARNING]
> The game code is subject to be completely rewritten as the engine develops.

## Versioning
Versions are currently formatted as follows: `[major].[feature].[patch]-[beta]` (e.g., `1.2.3`)<br>
- **Major Version:** Indicates the major version of the engine. It changes when there are significant updates that may include breaking changes or major new features.
- **Feature Count:** Current feature count of the major version. Each time a new feature is fully implemented, this number increases.
- **Patch Version:** Small updates, additions to parts of a feature, or bug fixes.
- **Beta Version:** Preliminary update that may include minor changes, early-stage features, or quick fixes. These versions are not fully tested and may be unstable, potentially leading to crashes. They are primarily used to preserve the current state of the engine and are not recommended for regular use. If you want to use them, read the description of the commit of that version before hand to make sure it won't cause problems in you code.

**Versioning Examples**
- **Version 0.1.0:** Game loop and texture rendering.
- **Version 0.1.1:** Keyboard Input System.
- **Version 0.2.0:** Input System.
- **Version 0.4.0-1:** Readme update
- **Version 1.0.0:** First release of the engine.

## Commits
Commits will be formated as follows:
- Name: new version of the engine
- Description: global idea of what this commit it

For more details about each commit, check the [Changelog.md](https://github.com/AntoineBastide47/GameEngine/blob/main/Changelog.md) file. Beta version will not be included in the changelog.

## Releases
Releases will be available starting from the first major version of the engine (Version 1.0.0).
Once the major version or feature count is incremented, a new release will be published.
Releases will be made available for Windows, macOS and Linux for now.<br>
Following every new major releases will be the version X.1.0 which will completely focus on increasing the performance and reducing the memory usage of the engine.
Previous version will have some basic optimisations to make the code run faster, but theses releases will be geared towards making the engine run as fast as possible.

## Modifying the engine
You are free to fork the project and modify it as much as you want to suit your needs. To do so, follow the `Getting Started`
guide. Once you are done, you will need to re-build the engine each time you modify it and then re-build and re-run your
game to link the modified engine to the game's executable.

## API Notices
> [!IMPORTANT]
> Naming conventions might be all over the place for now, but a standard will be implemented for the first release.

> [!IMPORTANT]
> APIs may change drastically in between versions as they may be rewritten in a better way as the engine grows.

> [!IMPORTANT]
> The build pipeline (Getting Started section) will also change in the near future, so make sure to check the changelog
> to see if there were any major changes to it.

> [!WARNING]
> The engine won't be backwards compatible as that would create too much overhead, increase the size of binaries and
> might reduce the performance of games.

## Limitations
> [!WARNING]
> I am currently building the engine on macOS, so the engine won't have the best support for Linux and Windows out of the box until version 1.0.0 is released.
> I will do my best to make so that the makefile and CMakeLists.txt should compile on Windows and Linux, but again, I won't make sure it works until version 1.0.0 as that is not my current priority.

## Contributing
Since this is a personal learning project, contributions are welcomed but may not be merged immediately. Feel free to fork the repository and experiment on your own.

## Learning Objectives
**C++ Mastery:** Enhance proficiency in modern C++ programming.<br>
**Game Development:** Understand the fundamentals of game engine development.<br>
**Software Architecture:** Apply design patterns and architectural principles.<br>
**Project Management:** Experience in planning, versioning, and documenting a software project.<br>

## License
This project is licensed under the Apache License Version 2.0.

## References
The following references include a collection of articles and videos that have served as the foundation for building my
engine (the list will evolve over time):
- OpenGL, Particle System, Audio: [Learn OpenGL](https://learnopengl.com/)
- Physics Engine: [Two-Bit Coding - Let's Make a Physics Engine](https://www.youtube.com/playlist?list=PLSlpr6o9vURwq3oxVZSimY8iC-cdd3kIs)<br>
> [!Important]
> Please note that strictly following these resources alone will not result in a complete game engine but can provide
> valuable insights and guidance for where/how to start. The code produced by following the tutorials will also not
> be identical to the one in the engine as it the references are used as starting points that I have built my engine upon.