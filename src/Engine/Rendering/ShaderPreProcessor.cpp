//
// ShaderPreProcessor.cpp
// Author: Antoine Bastide
// Date: 01/05/2025
//

#include <regex>
#include <sstream>

#include "Engine/Rendering/ShaderPreProcessor.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"

namespace Engine::Rendering {
  enum class ShaderType {
    None, Vertex, Fragment, Geometry
  };

  std::tuple<std::string, std::string, std::string> ShaderPreProcessor::preprocess(const std::string &shaderSource) {
    std::string vertexCode, fragmentCode, geometryCode;
    auto currentType = ShaderType::None;

    // Replace all the #using tags with their values
    std::ostringstream processedSource;
    std::istringstream stream(shaderSource);
    std::string line;

    while (std::getline(stream, line)) {
      std::regex simpleUsing(R"(#define\s+(\w+))");

      if (std::smatch match; std::regex_match(line, match, simpleUsing)) {
        // Inject the matrix layout group
        if (const std::string usingName = match[1].str(); usingName == "Matrices")
          processedSource << "layout(std140) uniform Matrices { mat4 uView; mat4 uProjection; mat4 uViewProjection; };\n";
          // Inject the SampleTexture function
        else if (usingName == "Textures") {
          processedSource
            << "uniform sampler2D textures["
            << Engine2D::Rendering::Renderer2D::MAX_TEXTURES
            << "];\n\nvec4 SampleTexture() { return texture(textures[int(vTextureData.z)], vTextureData.xy); }\n";
        }
      } else
        processedSource << line << '\n';
    }

    // Split the shaders into each stage
    std::istringstream splitStream(processedSource.str());
    while (std::getline(splitStream, line)) {
      if (line.find("#region VERTEX") != std::string::npos)
        currentType = ShaderType::Vertex;
      else if (line.find("#region FRAGMENT") != std::string::npos || line.find("#region PIXEL") != std::string::npos)
        currentType = ShaderType::Fragment;
      else if (line.find("#region GEOMETRY") != std::string::npos)
        currentType = ShaderType::Geometry;
      else {
        switch (currentType) {
          case ShaderType::Vertex: vertexCode += line + '\n';
            break;
          case ShaderType::Fragment: fragmentCode += line + '\n';
            break;
          case ShaderType::Geometry: geometryCode += line + '\n';
            break;
          default: break;
        }
      }
    }

    return std::make_tuple(vertexCode, fragmentCode, geometryCode);
  }
}
