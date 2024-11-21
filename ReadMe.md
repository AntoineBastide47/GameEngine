# 2D Game Engine Project
This game engine is a personal project I'm working on to learn more about game development and C++. It's designed for 2D games and aims to provide a simple yet powerful framework for building interactive applications. The project is both a fun endeavor and an educational journey into the world of game engine architecture.

## Current Features
**Rendering Engine:** Efficient rendering of sprites and textures.<br>
**Entity Management:** Organize game objects and their behaviors.<br>
**Input Handling:** Support for keyboard and mouse input.<br>

## Future Features
To check out what I plan to add to the engine in the future, or what I am currently working on, check out this link:
[Trello Board](https://trello.com/invite/b/67376d9fff131de8914e5da1/ATTI005d420929932a037057431249289ba6283CFF05/game-engine)

## Versioning
Versions are currently formatted as follows: `[major].[feature].[patch]` (e.g., `1.2.3`)<br>
- **Major Version:** The first number indicates the major version of the engine. It changes when there are significant updates that may include breaking changes or major new features.
- **Feature Count:** The second number represents the current feature count. Each time a new feature is fully implemented, this number increases.
- **Patch Version:** The last number signifies small updates, additions to parts of a feature, or bug fixes.

**Versioning Examples**
- **Version 0.1.0:** Game loop and texture rendering.
- **Version 0.1.1:** Keyboard Input System.
- **Version 0.2.0:** Input System.
- **Version 1.0.0:** First full version of the engine.

## Getting Started
To get started with the engine:

#### 1. Clone the Repository:
```bash
git clone https://github.com/AntoineBastide47/GameEngine.git
```
#### 2. Build the Engine:
```bash
cd GameEngine
make install_dependencies build_lib
```
#### 3. Create a Sample Game:
```bash
make create_project
```

#### 4. Running the game
```bash
cd your/project/location
make refresh build_game_run
```
The refresh command will copy the engine files into your game project's directory.
If everything work correctly, you should see a black screen appear once the build phase ended.

## Learning Objectives
**C++ Mastery:** Enhance proficiency in modern C++ programming.<br>
**Game Development:** Understand the fundamentals of game engine development.<br>
**Software Architecture:** Apply design patterns and architectural principles.<br>
**Project Management:** Experience in planning, versioning, and documenting a software project.<br>

## Limitations
I am currently building the engine on MacOS, so the engine wont have the best support for Linux and Windows out of the box,
but I will try my best to make it as cross-platform as possible

## Contributing
Since this is a personal learning project, contributions are welcomed but may not be merged immediately. Feel free to fork the repository and experiment on your own.

## License
This project is licensed under the Apache License Version 2.0.

## Contact
For any idea suggestions, please open post a comment on the Trello Board linked above.<br>
For any code suggestions, please open an issue on Github.