// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_RENDERABLE2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Rendering::Renderable2D>("Engine2D::Rendering::Renderable2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(renderOrder, format, json["renderOrder"]);\
      Engine::Reflection::_e_saveImpl(renderType, format, json["renderType"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(renderOrder, format, json.At("renderOrder"));\
      Engine::Reflection::_e_loadImpl(renderType, format, json.At("renderType"));\
    }\
  }\
  RENDER_RENDERABLE2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_RENDERABLE2D
#else
#define RENDER_RENDERABLE2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(renderOrder, "Render Order", readOnly);\
    return changed;\
  }
#endif

  #define REFLECT_RENDERABLETYPE\
  static inline const bool _reg_RENDERABLETYPE = [] {\
    Engine::Reflection::ReflectionFactory::RegisterEnum<Engine2D::Rendering::Renderable2D::RenderableType>(\
    "Engine2D::Rendering::Renderable2D::RenderableType", {\
      std::pair<std::string, Engine2D::Rendering::Renderable2D::RenderableType>{"None", Engine2D::Rendering::Renderable2D::RenderableType::None},\
      std::pair<std::string, Engine2D::Rendering::Renderable2D::RenderableType>{"SpriteRenderer", Engine2D::Rendering::Renderable2D::RenderableType::SpriteRenderer},\
      std::pair<std::string, Engine2D::Rendering::Renderable2D::RenderableType>{"ParticleSystem", Engine2D::Rendering::Renderable2D::RenderableType::ParticleSystem},\
      }\
    );\
    return true;\
  }();
