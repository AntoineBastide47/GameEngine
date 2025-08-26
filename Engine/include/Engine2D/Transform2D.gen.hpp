// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_TRANSFORM2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Transform2D>("Engine2D::Transform2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(worldPosition, format, json["worldPosition"]);\
      Engine::Reflection::_e_saveImpl(rotation, format, json["rotation"]);\
      Engine::Reflection::_e_saveImpl(worldRotation, format, json["worldRotation"]);\
      Engine::Reflection::_e_saveImpl(scale, format, json["scale"]);\
      Engine::Reflection::_e_saveImpl(worldScale, format, json["worldScale"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(worldPosition, format, json["worldPosition"]);\
      Engine::Reflection::_e_loadImpl(rotation, format, json["rotation"]);\
      Engine::Reflection::_e_loadImpl(worldRotation, format, json["worldRotation"]);\
      Engine::Reflection::_e_loadImpl(scale, format, json["scale"]);\
      Engine::Reflection::_e_loadImpl(worldScale, format, json["worldScale"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(position, "Position", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(rotation, "Rotation", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(scale, "Scale", readOnly);\
    return changed;\
  }\
  private: 
} // namespace Engine::Reflection
