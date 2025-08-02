//
// SceneResources.cpp
// Author: Antoine Bastide
// Date: 28.07.2025
//

#include <set>

#include "Engine2D/SceneManagement/SceneResources.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Reflection/Deserializer.hpp"
#include "Engine/Reflection/Serializer.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"

#define SHADER_KEY "shaders"
#define TEXTURE_KEY "textures"
#define SPRITE_KEY "sprites"

namespace Engine2D {
  void SceneResources::OnSerialize(const Engine::Reflection::Format format, Engine::JSON &json) const {
    std::set<Shader *> shaders;
    std::set<Texture *> textures;
    std::set<Sprite *> sprites;

    for (const auto renderer: renderables) {
      shaders.insert(renderer->GetShader());
      textures.insert(renderer->GetSprite()->texture);
      sprites.insert(renderer->GetSprite());
    }

    if (format == Engine::Reflection::Format::JSON) {
      int i = 0;
      json[SHADER_KEY].ReserveAndResize(shaders.size());
      for (const auto shader: shaders)
        json.At(SHADER_KEY).At(i++) = {shader->Name(), shader->Path()};

      int j = 0;
      json[TEXTURE_KEY].ReserveAndResize(textures.size());
      for (const auto texture: textures)
        json.At(TEXTURE_KEY).At(j++) = {texture->Name(), texture->Path(), texture->Transparent()};

      int k = 0;
      json[SPRITE_KEY].ReserveAndResize(sprites.size());
      for (const auto sprite: sprites)
        json.At(SPRITE_KEY).At(k++) = {
          sprite->Name(),
          sprite->texture->Name(),
          Engine::Reflection::Serializer::ToJson(sprite->rect),
          Engine::Reflection::Serializer::ToJson(sprite->pivot),
          sprite->pixelsPerUnit,
          sprite->transparent
        };
    }
  }

  void SceneResources::OnDeserialize(const Engine::Reflection::Format format, const Engine::JSON &json) {
    if (format == Engine::Reflection::Format::JSON) {
      for (const auto &shaderJSON: json.At(SHADER_KEY).GetArray())
        Engine::ResourceManager::LoadShader(shaderJSON.At(0).GetString(), shaderJSON.At(1).GetString());
      for (const auto &textureJSON: json.At(TEXTURE_KEY).GetArray())
        Engine::ResourceManager::LoadTexture2D(textureJSON.At(0).GetString(), textureJSON.At(1).GetString())
            ->transparent = textureJSON.At(2).GetBool();
      for (const auto &spriteJSON: json.At(SPRITE_KEY).GetArray()) {
        Sprite *sprite = Engine::ResourceManager::GetSprite(spriteJSON.At(0).GetString());
        if (!sprite)
          sprite = Engine::ResourceManager::CreateSprite(spriteJSON.At(0).GetString(), spriteJSON.At(1).GetString());

        sprite->rect = Engine::Reflection::Deserializer::FromJson<glm::vec4>(spriteJSON.At(2));
        sprite->pivot = Engine::Reflection::Deserializer::FromJson<glm::vec2>(spriteJSON.At(3));
        sprite->pixelsPerUnit = static_cast<float>(spriteJSON.At(4).GetNumber());
        sprite->transparent = spriteJSON.At(5).GetBool();
      }
    }
  }
} // Engine2D
