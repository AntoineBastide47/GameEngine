#ifndef {{PROJECT_NAME}}_H
#define {{PROJECT_NAME}}_H

#include <2D/Game2D.h>

class {{PROJECT_NAME}} : public Engine2D::Game2D {
  public:
    {{PROJECT_NAME}}(int width, int height) : Game2D(width, height, "{{PROJECT_NAME}}") {};

    void Initialize() override;
};


#endif //{{PROJECT_NAME}}_H
