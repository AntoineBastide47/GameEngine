// Auto-generated – DO NOT EDIT

#pragma once

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
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(name, format, json.At("name"));\
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(parentsActive, format, json.At("parentsActive"));\
      Engine::Reflection::_e_loadImpl(isStatic, format, json.At("isStatic"));\
      Engine::Reflection::_e_loadImpl(destroyed, format, json.At("destroyed"));\
    }\
  }\
  RENDER_ENTITY2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_ENTITY2D
#else
#define RENDER_ENTITY2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(name, "Name", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(active, "Active", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(parentsActive, "Parents Active", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isStatic, "Is Static", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(destroyed, "Destroyed", readOnly);\
    return changed;\
  }
#endif