// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_COMPONENT2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Component2D>("Engine2D::Component2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
    }\
  }\
  RENDER_COMPONENT2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_COMPONENT2D
#else
#define RENDER_COMPONENT2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    return false;\
}
#endif