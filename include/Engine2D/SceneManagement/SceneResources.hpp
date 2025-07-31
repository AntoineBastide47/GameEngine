//
// SceneResources.hpp
// Author: Antoine Bastide
// Date: 28.07.2025
//

#ifndef SCENE_RESOURCES_HPP
#define SCENE_RESOURCES_HPP

#include <string>
#include <unordered_set>

#include "Engine/Reflection/Reflectable.hpp"
#include "SceneResources.gen.hpp"

namespace Engine2D {
  class ParticleSystemRegistry2D;

  namespace Rendering {
    class Renderer2D;
    class Renderable2D;
  }
}

template<> struct std::hash<std::pair<std::string, std::string>> {
  size_t operator()(const std::pair<std::string, std::string> &p) const noexcept {
    const size_t h1 = hash<std::string>()(p.first);
    const size_t h2 = hash<std::string>()(p.second);
    return h1 ^ h2 << 1;
  }
};

struct PairHash {
  template<typename T> size_t operator()(const std::pair<std::pair<std::string, std::string>, T *> &p) const {
    const size_t h1 = std::hash<std::string>()(p.first.first);
    const size_t h2 = std::hash<std::string>()(p.first.second);
    const size_t h3 = std::hash<T *>()(p.second);
    return h1 ^ h2 << 1 ^ h3 << 2;
  }
};

namespace Engine {
  class ResourceManager;

  namespace Rendering {
    class Texture;
    class Shader;
  }
}

namespace Engine2D {
  namespace Rendering {
    class Sprite;
  }

  struct ResourceData : Engine::Reflection::Reflectable {
    SERIALIZE_RESOURCEDATA
      std::string name;
      std::string path;

      bool operator==(const ResourceData &other) const;
  };

  struct SpriteData : ResourceData {
    SERIALIZE_SPRITEDATA
      std::string textureName;
      glm::vec4 rect;
      glm::vec2 pivot;
      float pixelsPerUnit;

      bool operator==(const SpriteData &other) const;
  };

  class SceneResources final : public Engine::Reflection::Reflectable {
    SERIALIZE_SCENERESOURCES
      friend class Scene;
      friend class Rendering::Renderer2D;
      friend class Engine::ResourceManager;
      friend class ParticleSystemRegistry2D;
    public:
      void OnSerialize(Engine::Reflection::Format format, Engine::JSON &json) const override;
      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    private:
      std::vector<Rendering::Renderable2D *> renderables;
  };
} // Engine2D

#endif //SCENE_RESOURCES_HPP
