// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_COLOR _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine::Rendering::Color>("Engine::Rendering::Color");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(r, format, json["r"]);\
      Engine::Reflection::_e_saveImpl(g, format, json["g"]);\
      Engine::Reflection::_e_saveImpl(b, format, json["b"]);\
      Engine::Reflection::_e_saveImpl(a, format, json["a"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(r, format, json.At("r"));\
      Engine::Reflection::_e_loadImpl(g, format, json.At("g"));\
      Engine::Reflection::_e_loadImpl(b, format, json.At("b"));\
      Engine::Reflection::_e_loadImpl(a, format, json.At("a"));\
    }\
  }\
  RENDER_COLOR\
  private: 


#if !ENGINE_EDITOR
#define RENDER_COLOR
#else
#define RENDER_COLOR\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(r, "R", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(g, "G", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(b, "B", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(a, "A", readOnly);\
    return changed;\
  }
#endif