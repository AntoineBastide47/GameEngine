// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_SPRITE _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Rendering::Sprite>("Engine2D::Rendering::Sprite");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(rect, format, json["rect"]);\
      Engine::Reflection::_e_saveImpl(pivot, format, json["pivot"]);\
      Engine::Reflection::_e_saveImpl(pixelsPerUnit, format, json["pixelsPerUnit"]);\
      Engine::Reflection::_e_saveImpl(transparent, format, json["transparent"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(rect, format, json["rect"]);\
      Engine::Reflection::_e_loadImpl(pivot, format, json["pivot"]);\
      Engine::Reflection::_e_loadImpl(pixelsPerUnit, format, json["pixelsPerUnit"]);\
      Engine::Reflection::_e_loadImpl(transparent, format, json["transparent"]);\
    }\
  }\
  private: 
} // namespace Engine::Reflection
