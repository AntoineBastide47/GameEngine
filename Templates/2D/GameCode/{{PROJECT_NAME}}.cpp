#include <Input/Keyboard.h>

#include "{{PROJECT_NAME}}.h"

void {{PROJECT_NAME}}::Initialize() {
  Engine::Input::Keyboard::ESCAPE += [this](const Engine::Input::KeyboardAndMouseContext ctx) {
    Close(ctx);
  };
}