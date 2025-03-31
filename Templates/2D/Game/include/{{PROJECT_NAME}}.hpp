#ifndef {{PROJECT_NAME}}_H
#define {{PROJECT_NAME}}_H

#include <Engine2D/Game2D.hpp>

class {{PROJECT_NAME}} final : public Engine2D::Game2D {
  public:
    {{PROJECT_NAME}}(int width, int height);

    void OnInitialize() override;
};


#endif //{{PROJECT_NAME}}_H
