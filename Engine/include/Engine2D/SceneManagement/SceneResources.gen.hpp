// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_SCENERESOURCES _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::SceneResources>("Engine2D::SceneResources");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) {}\
  }\
  RENDER_SCENERESOURCES\
  private: 


#if !ENGINE_EDITOR
#define RENDER_SCENERESOURCES
#else
#define RENDER_SCENERESOURCES\
  bool _e_renderInEditor(const bool readOnly) override {\
    return false;\
}
#endif