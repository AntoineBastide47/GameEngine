//
// ShaderPreProcessor.hpp
// Author: Antoine Bastide
// Date: 01/05/2025
//

#ifndef SHADER_PREPROCESSOR_HPP
#define SHADER_PREPROCESSOR_HPP

#include <tuple>

namespace Engine {
  class ResourceManager;
}

namespace Engine2D {
  class Game2D;
}

namespace Engine::Rendering {
  class ShaderPreProcessor {
    friend class Engine2D::Game2D;
    friend class Engine::ResourceManager;

    static std::tuple<std::string, std::string, std::string> preprocess(const std::string &shaderSource);
  };
}

#endif //SHADER_PREPROCESSOR_HPP
