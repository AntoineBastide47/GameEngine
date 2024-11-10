//
// SpriteRenderer.h
// SpriteRenderer: 
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include "Shader.h"
#include "Texture2D.h"
#include "2D/Entity2D.h"
#include"2D/Components/Transform2D.h"

namespace Engine2D::Rendering {
  class SpriteRenderer {
    public:
      explicit SpriteRenderer(const Shader &shader);
      ~SpriteRenderer();

      void DrawSprite(const Entity2D *entity, glm::vec3 color = glm::vec3(1.0f));
    private:
      Shader shader;
      unsigned int quadVAO;

      void initRenderData();
  };
}

#endif //SPRITERENDERER_H
