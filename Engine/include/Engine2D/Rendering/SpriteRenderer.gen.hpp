// Auto-generated – DO NOT EDIT

#pragma once

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
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(renderOrder, format, json.At("renderOrder"));\
      Engine::Reflection::_e_loadImpl(renderType, format, json.At("renderType"));\
      Engine::Reflection::_e_loadImpl(color, format, json.At("color"));\
      Engine::Reflection::_e_loadImpl(flip, format, json.At("flip"));\
    }\
  }\
  RENDER_SPRITERENDERER\
  private: 


#if !ENGINE_EDITOR
#define RENDER_SPRITERENDERER
#else
#define RENDER_SPRITERENDERER\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(renderOrder, "Render Order", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(color, "Color", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(flip, "Flip", readOnly);\
    return changed;\
  }
#endif