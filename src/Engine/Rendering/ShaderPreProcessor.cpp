//
// ShaderPreProcessor.cpp
// Author: Antoine Bastide
// Date: 01/05/2025
//

#include <regex>
#include <sstream>

#include "Engine/Rendering/ShaderPreProcessor.hpp"

#include <fstream>

#include "Engine2D/Rendering/Renderer2D.hpp"

namespace Engine::Rendering {
  enum class ShaderType {
    None, Vertex, Fragment, Geometry
  };

  std::tuple<std::string, std::string, std::string> ShaderPreProcessor::preprocess(const std::string &shaderSource) {
    std::string vertexCode, fragmentCode, geometryCode;
    auto currentType = ShaderType::None;

    // Macro preprocessor
    std::unordered_map<std::string, std::string> userDefines;
    std::unordered_set<std::string> activeDefines;
    std::vector<bool> conditionStack;
    bool includeLine = true;

    // Replace all the #using tags with their values
    std::ostringstream processedSource;
    std::istringstream stream(shaderSource);
    std::string line;

    while (std::getline(stream, line)) {
      std::regex defineWithValue(R"(#define\s+(\w+)(?:\s+(.*))?)");

      if (std::smatch match; std::regex_match(line, match, defineWithValue)) {
        if (!includeLine)
          continue;

        const std::string name = match[1];
        const std::string value = match.size() > 2 ? match[2].str() : "1";
        activeDefines.insert(name);

        if (name == "Matrices")
          processedSource << "layout(std140) uniform Matrices { mat4 uView; mat4 uProjection; mat4 uViewProjection; };\n";
        else if (name == "Textures") {
          processedSource
            << "uniform sampler2D textures[" << Engine2D::Rendering::Renderer2D::MAX_TEXTURES << "];\n\n"
            << "vec4 SampleTexture() {\n"
            << "  int idx = int(vTextureData.z);\n"
            << "  if (idx == 0) return texture(textures[0], vTextureData.xy);\n";
          for (int i = 1; i < Engine2D::Rendering::Renderer2D::MAX_TEXTURES; ++i) {
            processedSource
              << "  else if (idx == " << i << ") return texture(textures[" << i << "], vTextureData.xy);\n";
          }
          processedSource
            << "  return vec4(1.0, 0.0, 1.0, 1.0);\n"
            << "}\n";
        } else {
          userDefines[name] = value;
        }
        continue;
      }

      if (line.find("#ifdef") == 0) {
        std::istringstream ss(line);
        std::string cmd, name;
        ss >> cmd >> name;
        conditionStack.push_back(includeLine);
        includeLine = includeLine && activeDefines.contains(name);;
        continue;
      }

      if (line.find("#ifndef") == 0) {
        std::istringstream ss(line);
        std::string cmd, name;
        ss >> cmd >> name;
        conditionStack.push_back(includeLine);
        includeLine = includeLine && !activeDefines.contains(name);
        continue;
      }

      if (line.find("#else") == 0) {
        if (!conditionStack.empty()) {
          bool parent = conditionStack.back();
          includeLine = parent && !includeLine;
        }
        continue;
      }

      if (line.find("#endif") == 0) {
        if (!conditionStack.empty()) {
          includeLine = conditionStack.back();
          conditionStack.pop_back();
        }
        continue;
      }

      if (includeLine)
        processedSource << line << '\n';
    }

    std::ostringstream expandedSource;
    std::istringstream expansionStream(processedSource.str());

    // Find and replace user defined macros
    while (std::getline(expansionStream, line)) {
      for (const auto &[macro, value]: userDefines) {
        size_t pos = 0;
        while ((pos = line.find(macro, pos)) != std::string::npos) {
          bool leftValid = pos == 0 || !std::isalnum(line[pos - 1]);
          if (bool rightValid = pos + macro.size() >= line.size() || !std::isalnum(line[pos + macro.size()]);
            leftValid && rightValid) {
            line.replace(pos, macro.size(), value);
            pos += value.size();
          } else {
            pos += macro.size();
          }
        }
      }
      expandedSource << line << '\n';
    }

    // Split the shaders into each stage
    std::istringstream splitStream(expandedSource.str());
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
