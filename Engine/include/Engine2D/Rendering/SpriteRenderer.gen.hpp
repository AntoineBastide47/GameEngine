// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_SPRITERENDERER _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Rendering::SpriteRenderer>("Engine2D::Rendering::SpriteRenderer");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(renderOrder, format, json["renderOrder"]);\
      Engine::Reflection::_e_saveImpl(renderType, format, json["renderType"]);\
      Engine::Reflection::_e_saveImpl(color, format, json["color"]);\
      Engine::Reflection::_e_saveImpl(flip, format, json["flip"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(renderOrder, format, json["renderOrder"]);\
      Engine::Reflection::_e_loadImpl(renderType, format, json["renderType"]);\
      Engine::Reflection::_e_loadImpl(color, format, json["color"]);\
      Engine::Reflection::_e_loadImpl(flip, format, json["flip"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(renderOrder, "RenderOrder", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(renderType, "RenderType", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(color, "Color", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(flip, "Flip", readOnly);\
    return changed;\
  }\
  private: 
} // namespace Engine::Reflection
