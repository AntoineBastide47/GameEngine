#include <Engine/Input/Keyboard.hpp>

#include "{{PROJECT_NAME}}.hpp"

{{PROJECT_NAME}}::{{PROJECT_NAME}}(const int width, const int height) : Game2D(width, height, "{{PROJECT_NAME}}") {}

void {{PROJECT_NAME}}::OnInitialize() {
  Engine::Input::Keyboard::ESCAPE += [this](const Engine::Input::KeyboardAndMouseContext ctx) {
    if (ctx.pressed)
      Quit();
  };
}