#include "{{PROJECT_NAME}}.h"

#include <Input/Keyboard.h>

void {{PROJECT_NAME}}::Initialize() {
  Engine::Input::Keyboard::ESCAPE += [this](const Engine::Input::KeyboardAndMouseContext ctx) {
    Close(ctx);
  };
}