// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_ANIMATION2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Animation::Animation2D>("Engine2D::Animation::Animation2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(frameCountX, format, json["frameCountX"]);\
      Engine::Reflection::_e_saveImpl(frameCountY, format, json["frameCountY"]);\
      Engine::Reflection::_e_saveImpl(frameOffsetX, format, json["frameOffsetX"]);\
      Engine::Reflection::_e_saveImpl(frameOffsetY, format, json["frameOffsetY"]);\
      Engine::Reflection::_e_saveImpl(currentFrame, format, json["currentFrame"]);\
      Engine::Reflection::_e_saveImpl(loop, format, json["loop"]);\
      Engine::Reflection::_e_saveImpl(reverse, format, json["reverse"]);\
      Engine::Reflection::_e_saveImpl(spriteName, format, json["spriteName"]);\
      Engine::Reflection::_e_saveImpl(vertical, format, json["vertical"]);\
      Engine::Reflection::_e_saveImpl(paused, format, json["paused"]);\
      Engine::Reflection::_e_saveImpl(completed, format, json["completed"]);\
      Engine::Reflection::_e_saveImpl(frameDuration, format, json["frameDuration"]);\
      Engine::Reflection::_e_saveImpl(speed, format, json["speed"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(frameCountX, format, json.At("frameCountX"));\
      Engine::Reflection::_e_loadImpl(frameCountY, format, json.At("frameCountY"));\
      Engine::Reflection::_e_loadImpl(frameOffsetX, format, json.At("frameOffsetX"));\
      Engine::Reflection::_e_loadImpl(frameOffsetY, format, json.At("frameOffsetY"));\
      Engine::Reflection::_e_loadImpl(currentFrame, format, json.At("currentFrame"));\
      Engine::Reflection::_e_loadImpl(loop, format, json.At("loop"));\
      Engine::Reflection::_e_loadImpl(reverse, format, json.At("reverse"));\
      Engine::Reflection::_e_loadImpl(spriteName, format, json.At("spriteName"));\
      Engine::Reflection::_e_loadImpl(vertical, format, json.At("vertical"));\
      Engine::Reflection::_e_loadImpl(paused, format, json.At("paused"));\
      Engine::Reflection::_e_loadImpl(completed, format, json.At("completed"));\
      Engine::Reflection::_e_loadImpl(frameDuration, format, json.At("frameDuration"));\
      Engine::Reflection::_e_loadImpl(speed, format, json.At("speed"));\
    }\
  }\
  RENDER_ANIMATION2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_ANIMATION2D
#else
#define RENDER_ANIMATION2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(frameCountX, "Frame Count X", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(frameCountY, "Frame Count Y", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(frameOffsetX, "Frame Offset X", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(frameOffsetY, "Frame Offset Y", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(currentFrame, "Current Frame", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(loop, "Loop", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(reverse, "Reverse", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(spriteName, "Sprite Name", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(vertical, "Vertical", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(paused, "Paused", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(completed, "Completed", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(frameDuration, "Frame Duration", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(speed, "Speed", readOnly);\
    return changed;\
  }
#endif