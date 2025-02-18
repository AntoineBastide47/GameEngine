#include <Engine/Input/Keyboard.h>

#include "{{PROJECT_NAME}}.h"

{{PROJECT_NAME}}::{{PROJECT_NAME}}(const int width, const int height) : Game2D(width, height, "{{PROJECT_NAME}}") {}

void {{PROJECT_NAME}}::OnInitialize() {
  Engine::Input::Keyboard::ESCAPE += [this](const Engine::Input::KeyboardAndMouseContext ctx) {
    Close(ctx);
  };
}