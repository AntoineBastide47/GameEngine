// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_SCENE _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Scene>("Engine2D::Scene");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(resources, format, json["resources"]);\
      Engine::Reflection::_e_saveImpl(entities, format, json["entities"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(resources, format, json.At("resources"));\
      Engine::Reflection::_e_loadImpl(entities, format, json.At("entities"));\
    }\
  }\
  RENDER_SCENE\
  private: 


#if !ENGINE_EDITOR
#define RENDER_SCENE
#else
#define RENDER_SCENE\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(resources, "Resources", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(entities, "Entities", readOnly);\
    return changed;\
  }
#endif