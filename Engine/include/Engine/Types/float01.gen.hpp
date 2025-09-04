// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_FLOAT01 _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine::float01>("Engine::float01");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(value, format, json["value"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(value, format, json.At("value"));\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(value, "Value", readOnly);\
    return changed;\
  }\
  private: 
} // namespace Engine::Reflection
