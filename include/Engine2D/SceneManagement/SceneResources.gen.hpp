// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_RESOURCEDATA _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::ResourceData>("Engine2D::ResourceData");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(name, format, json["name"]);\
      Engine::Reflection::_e_saveImpl(path, format, json["path"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(name, format, json["name"]);\
      Engine::Reflection::_e_loadImpl(path, format, json["path"]);\
    }\
  }

  #define SERIALIZE_SPRITEDATA _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::SpriteData>("Engine2D::SpriteData");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(name, format, json["name"]);\
      Engine::Reflection::_e_saveImpl(path, format, json["path"]);\
      Engine::Reflection::_e_saveImpl(textureName, format, json["textureName"]);\
      Engine::Reflection::_e_saveImpl(rect, format, json["rect"]);\
      Engine::Reflection::_e_saveImpl(pivot, format, json["pivot"]);\
      Engine::Reflection::_e_saveImpl(pixelsPerUnit, format, json["pixelsPerUnit"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(name, format, json["name"]);\
      Engine::Reflection::_e_loadImpl(path, format, json["path"]);\
      Engine::Reflection::_e_loadImpl(textureName, format, json["textureName"]);\
      Engine::Reflection::_e_loadImpl(rect, format, json["rect"]);\
      Engine::Reflection::_e_loadImpl(pivot, format, json["pivot"]);\
      Engine::Reflection::_e_loadImpl(pixelsPerUnit, format, json["pixelsPerUnit"]);\
    }\
  }

  #define SERIALIZE_SCENERESOURCES _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::SceneResources>("Engine2D::SceneResources");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) {}\
  }\
  private: 
} // namespace Engine::Reflection
