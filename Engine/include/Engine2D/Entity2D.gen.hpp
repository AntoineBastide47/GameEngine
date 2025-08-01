// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_ENTITY2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Entity2D>("Engine2D::Entity2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(name, format, json["name"]);\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(parentsActive, format, json["parentsActive"]);\
      Engine::Reflection::_e_saveImpl(isStatic, format, json["isStatic"]);\
      Engine::Reflection::_e_saveImpl(destroyed, format, json["destroyed"]);\
      Engine::Reflection::_e_saveImpl(transform, format, json["transform"]);\
      Engine::Reflection::_e_saveImpl(allComponents, format, json["allComponents"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(name, format, json["name"]);\
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(parentsActive, format, json["parentsActive"]);\
      Engine::Reflection::_e_loadImpl(isStatic, format, json["isStatic"]);\
      Engine::Reflection::_e_loadImpl(destroyed, format, json["destroyed"]);\
      Engine::Reflection::_e_loadImpl(transform, format, json["transform"]);\
      Engine::Reflection::_e_loadImpl(allComponents, format, json["allComponents"]);\
    }\
  }\
  private: 
} // namespace Engine::Reflection
