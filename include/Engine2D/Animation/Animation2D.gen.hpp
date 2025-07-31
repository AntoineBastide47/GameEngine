// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
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
      Engine::Reflection::_e_loadImpl(frameCountX, format, json["frameCountX"]);\
      Engine::Reflection::_e_loadImpl(frameCountY, format, json["frameCountY"]);\
      Engine::Reflection::_e_loadImpl(frameOffsetX, format, json["frameOffsetX"]);\
      Engine::Reflection::_e_loadImpl(frameOffsetY, format, json["frameOffsetY"]);\
      Engine::Reflection::_e_loadImpl(currentFrame, format, json["currentFrame"]);\
      Engine::Reflection::_e_loadImpl(loop, format, json["loop"]);\
      Engine::Reflection::_e_loadImpl(reverse, format, json["reverse"]);\
      Engine::Reflection::_e_loadImpl(spriteName, format, json["spriteName"]);\
      Engine::Reflection::_e_loadImpl(vertical, format, json["vertical"]);\
      Engine::Reflection::_e_loadImpl(paused, format, json["paused"]);\
      Engine::Reflection::_e_loadImpl(completed, format, json["completed"]);\
      Engine::Reflection::_e_loadImpl(frameDuration, format, json["frameDuration"]);\
      Engine::Reflection::_e_loadImpl(speed, format, json["speed"]);\
    }\
  }\
  private: 
} // namespace Engine::Reflection
