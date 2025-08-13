#ifndef {{PROJECT_NAME}}_H
#define {{PROJECT_NAME}}_H

#include <Engine2D/Game2D.hpp>

class {{PROJECT_NAME}} final : public Engine2D::Game2D {
  friend class Plugin;
  public:
    {{PROJECT_NAME}}(int width, int height);
};


#endif //{{PROJECT_NAME}}_H
