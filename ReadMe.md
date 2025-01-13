# Game Engine  [![License](https://img.shields.io/github/license/TheCherno/Hazel.svg)](https://github.com/AntoineBastide47/GameEngine/blob/main/LICENSE)
This game engine is a personal project I'm working on to learn more about game development and C++. It is currently
designed to run 2D games and aims to provide a simple yet powerful framework for building interactive applications.
The project is both a fun endeavor and an educational journey into the world of game engine architecture.

## Current Features
**Rendering Engine:** Efficient rendering of sprites and textures.<br>
**Entity Management:** Managing the lifetime of entities and their components.<br>
**Input Handling:** Keyboard, mouse and gamepad input.<br>
**Physics Engine:** Rigidbodies and collision responses.<br>

## Future Features
To check out what I plan to add to the engine in the future, or what I am currently working on, check out this link:
[Trello Board](https://trello.com/invite/b/67376d9fff131de8914e5da1/ATTI005d420929932a037057431249289ba6283CFF05/game-engine)

## Versioning
Versions are currently formatted as follows: `[major].[feature].[patch].[beta]` (e.g., `1.2.3`)<br>
- **Major Version:** Indicates the major version of the engine. It changes when there are significant updates that may include breaking changes or major new features.
- **Feature Count:** Current feature count of the major version. Each time a new feature is fully implemented, this number increases.
- **Patch Version:** Small updates, additions to parts of a feature, or bug fixes.
- **Beta Version:** Preliminary update that may include minor changes, early-stage features, or quick fixes. These versions are not fully tested and may be unstable, potentially leading to crashes. They are primarily used to preserve the current state of the engine and are not recommended for regular use. If you want to use them, read the description of the commit of that version before hand to make sure it won't cause problems in you code.

**Versioning Examples**
- **Version 0.1.0:** Game loop and texture rendering.
- **Version 0.1.1:** Keyboard Input System.
- **Version 0.2.0:** Input System.
- **Version 0.4.0.1:** Readme update
- **Version 1.0.0:** First release of the engine.

## Commits
Commits will be formated as follows:
- Name: new version of the engine
- Description: global idea of what this commit it
For more details about each commit, check the [Changelog.md](https://github.com/standard/standard/blob/master/CHANGELOG.md) file. Beta version will not be included in the changelog.

## Getting Started
To get started with the engine:

#### 1. Download the release zip file for your target OS
The zip releases will be available on GitHub starting from version 1.0.0 when the engine will be cross-platform
#### 2. Unzip the engine
#### 3. Open the uncompressed folder in the terminal and run
```bash
make create-project
```
#### 4. Running the game
```bash
cd your/project/location
make build run
```
If everything work correctly, you should see a black screen appear once the build phase ended.

#### 5. Make command information
```bash
make
```
or
```bash
make help
```
These two commands will give you descriptions for each available make command, be it in the engine or game folder.

## Modifying the engine
To modify the engine:
### 1. Clone the Repository:
```bash
git clone https://github.com/AntoineBastide47/GameEngine.git
```
### 2. Modify the engine as you see fit
### 3. Build the engine
#### 3.1 Debug build
```bash
make build-debug
```
This builds a debug version of the engine.
#### 3.2 Release build
```bash
make build-release
```
This builds an optimized version of the engine, and makes it ready for release by creating the necessary zip file.

## Learning Objectives
**C++ Mastery:** Enhance proficiency in modern C++ programming.<br>
**Game Development:** Understand the fundamentals of game engine development.<br>
**Software Architecture:** Apply design patterns and architectural principles.<br>
**Project Management:** Experience in planning, versioning, and documenting a software project.<br>

## Examples
For example games that implement the engine, check out: https://github.com/AntoineBastide47/TestGames
Warning: the game code is subject to change as the engine develops.

## Limitations
I am currently building the engine on macOS, so the engine won't have the best support for Linux and Windows out of the box,
but I am planning to make the engine cross-platform, including Windows and Linux for version 1.0.0.<br>
The engine won't be backwards compatible as that would create too much overhead, increase the size of binaries and
might reduce the performance of games.

## Contributing
Since this is a personal learning project, contributions are welcomed but may not be merged immediately. Feel free to fork the repository and experiment on your own.

## License
This project is licensed under the Apache License Version 2.0.

## References
The following references include a collection of articles and videos that have served as the foundation for building my
engine. This list will continue to evolve over time. Please note that strictly following these resources alone will not
result in a complete game engine but can provide valuable insights and guidance for where/how to start.
 - OpenGL: [Learn OpenGL](https://learnopengl.com/)
 - Physics Engine: [Two-Bit Coding - Let's Make a Physics Engine](https://www.youtube.com/playlist?list=PLSlpr6o9vURwq3oxVZSimY8iC-cdd3kIs)